#include <ODBC/ODBCQuery.h>
#include <ODBC/ODBCQueryManager.h>

#ifdef HAVE_STRSTREAM
#include <strstream>
#else
#ifdef HAVE_STRSTREAM_H
#include <strstream.h>
#else
#error "not defined HAVE_STRSTREAM or HAVE_STRSTREAM_H"
#error "check configuration parameters"
#endif
#endif

#include <new>
#include <exception>
#include <set>
using namespace std;
using namespace UAKGQuery2;

typedef set<string> names_set;


ODBCQuery::ODBCQuery(const char* query_text, 
               ODBCQueryManager* odbcQueryManager_p,
               const char* szflags
              )
    :UAKGQueryBase(query_text,odbcQueryManager_p,szflags)
    ,dbFormatHandler_(NULL)
  {
   odbcQueryManager_p_=odbcQueryManager_p;
   xa_ = false;
   statementHanlde=SQL_NULL_HSTMT;
  }

ODBCQuery::~ODBCQuery()
{
 try {
   close();
 }catch(const UAKGQuery2::QueryProcessingError& ex){
   getLogger().warnings() << "Error during destroy:" << ex.why << std::endl;
 }
}
  
void ODBCQuery::prepare_internal()  
{

 SQLHANDLE envh=getEnvHandle();
 SQLHANDLE cnth=getConnectionHandle();

    if (statementHanlde == SQL_NULL_HSTMT) {
      try {
        checkError(SQLAllocHandle( SQL_HANDLE_STMT, cnth, &connectionHandle_));
      }catch(const QueryProcessingError& ){
        connectionHandle = SQL_NULL_HSTMT;
        throw;
      }
    }

    checkError(SQLPrepare(connectionHandle_,queryText_,strlen(queryText_)));

    checkError(OCIAttrGet((dvoid*)ociStmt_p_,(ub4)OCI_HTYPE_STMT,
                       &stmtType_,(ub4 *)NULL,(ub4)OCI_ATTR_STMT_TYPE,
                       ociError_p()
           )          );
    if (stmtType_ == OCI_STMT_SELECT)  {
       selectFlg_ = true;
    }


    RecordSet_var rs = getParamsBindDescription();

    setParamsBinds(rs->getNColumns());


    rebind();

   
}
  


void ODBCQuery::execute_internal()
{

  // TODO: handle transactions.

    int paramsNRows = paramsBindSet_p_->getNRows();
    if (paramsNRows==0 || paramsBindSet_p_->getNColumns()==0) {
       paramsNRows = 1;
    }

    rebind();

    SQLRETURN state = SQLExecute(statementHandle_);

    if (err != SQL_NO_DATA) checkError(err);
    else retrieved_ = true;

    
    if(isSelect_) { 
      RecordSet_var r_rs = buildResultBindDescription();
      setResultBind(r_rs);

    }else{
      retrievedNRows_ = rowCount;
    }

    SQLINTEGER rowCount;
    SQLRESULT result = SQLRowCount(statementHandle_,&rowCount);

    retrievedNRows_ = rowCount;
    executed_ = true;

}

CORBA::Boolean ODBCQuery::retrieve_internal()
{
#ifdef CORBA_HAVE_OTS
#ifndef USE_OB_XA
  // TODO: transaction
#endif
#endif
    if (fetchedNRows_ < retrievedNRows_)
       return !retrieved_;  // i. e. we must "physically" call retrieve_internal
                            // only if we fetch all before.
    if (!selectFlg_) return false;
    if (!retrieved_)
    {
        resultBindSet_p_->initialize();

        // TODO: use SQLExtendedFetch if driver supports one.

        SQLRETURN result = SQLFetchScroll(statementHandle_,SQL_FETCH_NEXT,retrievedNRows_);
        if (result == SQL_NO_DATA) retrieved_ = true;
        else checkError(result);


    }else{
        return false;
 //           rebind();
 //           execute_rebinded();
 //           return true;
 //      }
    }

    return !retrieved_;
}

RecordSet* ODBCQuery::buildResultBindDescription() 
{


 RecordSet_var resultSet = new RecordSetImpl();
 OCIParam* ociParam_p = NULL;
 ub2       stmtType;
 ub4       paramCounter=1;  
 ub4       paramCount;  
 ub2       dtype;
 ub2       fieldLength;
 text*     columnName;
 ub4       columnNameLength;
 names_set used_names;
 unsigned short  field_index = 1;
    

 selectFlg_=determintatSelectFlg() // TODO: scan SQL :(

 if (isSelect_) {

   int paramCount =  SQLNumResultCols(statementHandle_);

   SQLHANDLE descrHandle;

   descrHandle=SQLAllocHandle(SQL_DESC_HANDLE,getConnectionHandle(),&descrHandle);

   SQLSMALLINT     fieldIdentifier;
   char            buff[DESCR_VALUE_FIELD_LEN];
   SQLINTEGER      fieldIdLen;

   for(paramCounter=1;paramCounter <= paramCount; ++paramCounter)
   {

     
     SQLGetDescField(descrHandle,paramCount,fieldIdentifier,&buff,DESCR_VALUE_FIELD_LEN,&fieldIdLen);
     

     // TODO: analyze and build.
     
     char* name = CORBA::string_alloc(columnNameLength+1);
     strncpy(name,(const char*)columnName,columnNameLength);
     name[columnNameLength]='\0';

     // To prevent similar field names all recurring names must be renamed to name + index
     // Name creation algorithm is according to interbase name creation algorithm
     names_set::iterator n_it = used_names.find(name);
     if(n_it!=used_names.end()){
        unsigned short n = field_index++;
        char buff[6];  // max short consist of 5 digits
        buff[5] = '\0';
        char* start = buff+5;
        do {
            *--start = '0' + (n % 10);
            n /= 10;
        } while (n);
        CORBA::string_free(name);
        int buff_len = strlen(start);
        name = CORBA::string_alloc(columnNameLength+1 + buff_len);
        strncpy(name,(const char*)columnName,columnNameLength);
        name[columnNameLength]='\0';
        strncat(name, start, buff_len);
        columnNameLength += buff_len;
        name[columnNameLength] = '\0';
     }
     used_names.insert(name);

     try {
        CORBA::ULong  col = resultSet->getNColumns();

        resultSet->addColumn(name, ftype); 
        CORBA::string_free(name);

        if ( resultSet->haveFieldSizeAt(col) ) {
             resultSet->setFieldSizeAt(col,fieldLength);
        }

        if ( resultSet->haveNumericFieldPropertiesAt(col) ) {
           resultSet->setNumericFieldPropertiesAt( col, precision, scale );
        }
     } catch(const NameInUse& ex) {
        CORBA::string_free(name);
        throwQueryProcessingError(400, "Error: Name in use", false);    
     } catch(const InvalidColumn& ex) {
        throwQueryProcessingError(400, "Error: Invalid column", false); 
     } catch(const InvalidPrecision& ex) {
         throwQueryProcessingError(400, "Error: Invalid precision", false);  
     } catch(const UAKGQuery2::InvalidFieldType& ex) {
         throwQueryProcessingError(400, "Error: Invalid field type", false); 
     }
   }
 }
 return resultSet._retn();
}


void ODBCQuery::close()
{
 if (!closed_) {
   UAKGQueryBase::close();
   if (statementHandle_!=SQL_NULL_HSTMT) {  
     if (isSelect_) {
        SQLCloseCursor(statementHandle_);
        statementHandle_=SQL_NULL_HSTMT;
   }
   if (dbFormatHandler_ != NULL) {
       delete dbFormatHandler_;
       dbFormatHandler_ = NULL;
   }
 }
 closed_ = true;
}



void ODBCQuery::checkError(sword status)
{
    switch(status){
       case SQL_SUCCESS:
          queryError_.dbCode = queryError_.code = 0;
          queryError_.why = CORBA::string_dup(""); 
          break;
       case SQL_SUCCESS_WITH_INFO:
          //setWarning(status,"warning: OCI_SUCCESS_WITH_INFO");
          break;
       case SQL_NEED_DATA:
          throwQueryProcessingError((CORBA::ULong)3,"Error: SQL_NEED_DATA", false);
          break;
       case SQl_NO_DATA:
          setWarning(status,"warning: OCI_NO_DATA");
          break;
       case SQL_ERROR:
          throwQueryProcessingError(status,"Error: SQL_ERROR",true);
          break;
       case SQL_INVALID_HANDLE:
          throwQueryProcessingError(status,"Error: SQL_INVALID_HANDLE",false);
          break;
       default:
          throwQueryProcessingError(status,"Error: Unknown Error",false);
          break;
    }
}

#ifdef  _MSC_VER
#pragma warning(disable:4715)           // disable not all path return value
#endif  // _MSC_VER





PortableServer::ObjectId* ODBCQuery::createTemporaryLobId(int rowCount, int colCount)
{

   //  OnjectId

 // TODO: pack int ObjecfId roowCount, colCount and connectioNHamdle_; 



   return createLobId(buff);
}

PortableServer::ObjectId* ODBCQuery::createLobId(OCILobLocator* lobData)
{	
  PortableServer::ObjectId_var oid=new PortableServer::ObjectId();
  oid->length(sizeof(OCILobLocator*));
  memcpy(oid->get_buffer(),&lobData,sizeof(OCILobLocator*));
  return oid._retn();
}

