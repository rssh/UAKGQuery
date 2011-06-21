#include <ODBC/ODBCQueryManager.h>
#include <ODBC/ODBCQuery.h>

#include <ODBC/ODBCLobs.h>
#include <UAKGQuerySingleton.h>

#ifdef CORBA_HAVE_OTS
#ifdef USE_OB_XA
#include <OB/OTS.h>
#include <OB/OTSXA.h>
#endif
#endif

#include <sstream>


using namespace std;
using namespace UAKGQuery2;


ODBCQueryManager::ODBCQueryManager()
   :connectionHandle_(SQL_NULL_HDBC),
{
}

ODBCQueryManager::~ODBCQueryManager()
{
 if (is_open() ) close();
}

void ODBCQueryManager::init( 
                             DBConnectionManagers_impl* dbConnectionManagers_p,
			     const DBConnectionParams* params_p)
{
 UAKGQueryManagerBase::init(dbConnectionManagers_p, params_p);
 connectionHandle_ = SQL_NULL_HDBC;
 // TODO: parse properties.
 SQLRETURN retval = ODBCAllocHandle(SQL_HANDLE_DBC,owner_ptr_->getEnvHandle(),&connetionHandle_);
 // TODO: check errors,
}

CORBA::Long ODBCQueryManager::get_next_sequence_value_internal(const char* sequence)
          	throw (  UAKGQuery2::QueryProcessingError,
            	     UAKGQuery2::QueryFlagInvalid,
                	 CORBA::SystemException)
{
    // TODO: get sql from properties for associated database.
    
    //RecordSet_var rs = evaluate_e(nextSequenceValueSQL.str(), "");
    //CORBA::String_var tmp = rs->getAsStringAt(0,0,"0");
    //long retval = atoi(tmp);
    return retval;
}
 
void ODBCQueryManager::close()
{
 if (connectionHandle_!=SQL_NULL_HDBC) {
    SQLDisconnect(connectionHandle_);
    SQLFreeHandle(SQL_HANDLE_DBC,connectionHandle_);
    connectionHandle_=SQL_NULL_HDBC;
 }
}

UAKGQueryBase*  ODBCQueryManager::createUAKGQuery(
                           const char* query_text, const char* flags)
		          throw(UAKGQuery2::QueryFlagInvalid,
	                  CORBA::SystemException)
{
  return new ODBCQuery(query_text,this,flags);
}

/**
 *  return sql adapted for custom driver
**/
char*  ODBCQueryManager::normalize_sql(const char* sql)
{
  return CORBA::string_dup(sql);
}


UAKGLobBase* ODBCQueryManager::createBlobReadingServant()
{
 UAKGLobBase* retval = new ODBCBlobForReading();
 retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* ODBCQueryManager::createClobReadingServant()
{
 UAKGLobBase* retval= new ODBCClobForReading();
 retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* ODBCQueryManager::createWclobReadingServant()
{
  UAKGLobBase* retval = new ODBCWclobForReading();
  retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* ODBCQueryManager::createBlobWritingServant()
{
 UAKGLobBase* retval = new ODBCBlobForWriting();
 retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* ODBCQueryManager::createClobWritingServant()
{
 UAKGLobBase* retval = new ODBCClobForWriting();
 retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* ODBCQueryManager::createWclobWritingServant()
{
 UAKGLobBase* retval = new ODBCWclobForWriting();
 retval->setOwnerPtr(this);
 return retval;
}


void ODBCQueryManager::checkError(SQLRETURN status)
{
    switch(status){
       case SQL_SUCCESS:
          break;
       case SQL_SUCCESS_WITH_INFO:
          break;
       case SQL_ERROR_DATA:
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

void ODBCQueryManager::fillDBError(QueryProcessingError& queryError, SQLSMALLINT handleType,SQLHandle hanlde)
{
  char stateCodeBuffer[5];
  char messageBuff[513];
  SQLSMALLINT  messageLen;
  SQLINTEGER   nativeErrorCode;
  sb4  errcode = 0;
  memset(errbuff,0,512);
  (void)SQLGetDiagRec(handleType,handle,1,stateCodeBuffer, &nativeErrorCode,messageBuff,512,&messageLen);
  queryError.dbCode=nativeErrorCode;
  queryError.why=CORBA::string_dup((char*)errbuff);
  queryError.code=errcode;
}

void ODBCQueryManager::throwQueryProcessingError(CORBA::ULong errorCode,const char* msg,bool dbFlg, SQLSMALLINT handleType, SQLHANDLE handle)
{
 QueryProcessingError ex;
 if (dbFlg) {
   if (connectionHandle_==SQL_NULL_HDBC) {
     ex.why = CORBA::string_dup(msg);     
     ex.dbCode=-1;
   }else {
      fillDBError(ex,handleType,handle);
   }
 }else{
   ex.why=CORBA::string_dup(msg);
   ex.dbCode=0;
 }
 {
  GradSoft::Logger::ErrorLocker guard(errors());
  errors() << "QueryProcessingError: " <<
               errorCode << "," << ex.dbCode << "\n";
  errors() << "msg: " <<  msg << "\n";
  errors() << "non-sql operation "  << std::endl;
 }
 throw ex;
}

