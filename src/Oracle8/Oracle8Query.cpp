#include <Oracle8/Oracle8Query.h>
#include <Oracle8/Oracle8QueryManager.h>

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

#define MAX_CLOB_SIZE  (1024*8*8 - 3)


Oracle8Query::Oracle8Query(const char* query_text, 
               Oracle8QueryManager* oracle8QueryManager_p,
               const char* szflags
              )
    :UAKGQueryBase(query_text,oracle8QueryManager_p,szflags)
    ,dbFormatHandler_(NULL)
  {
   oracle8QueryManager_p_=oracle8QueryManager_p;
   xa_ = false;
   useStrClob_ = false;
   OCIError* ociError = NULL;
   try {
     CORBA::ULong allocMode=0;
#ifdef USE_OB_XA
     if (flags_.get_sz("XA")!=NULL) {
        allocMode |= OB_XA_ALLOC_MODE; 
        xa_ = true;
     }
#endif
     if (flags_.get_sz("STR_CLOB")!=NULL) {
        useStrClob_ = true;
     }
     // TODO: set alloc mode, depends from flags.
     handles_p_=oracle8QueryManager_p->getSLHndlsPool_p()->alloc(allocMode);
   }catch(AllocHandleException ex){
     //
     // we can't throw exception from constructor 
     // (limitations of some compilers), so simple fill exception.
     //
     error_ = true;
     ociError=ex.ociError;
   }catch(const SessionBeginException& ex){
     error_ = true;
     ociError = ex.ociError;
   }catch(TransactionJoinException ex){
     ostrstream ostr;
     ostr << "Join to global transaction failed:" << ex.msg << '\0';
     setQueryProcessingError(1002,ostr.str(),false);
     ostr.rdbuf()->freeze(0);
   }catch(CORBA::TRANSACTION_REQUIRED){
     ociError = NULL; 
     setQueryProcessingError(1000,"TRANSACTION_REQUIRED",false);
   }catch(CORBA::NO_IMPLEMENT){
     ociError = NULL; 
     setQueryProcessingError(1001,"OTS handling not implemented",false);
   }
   if (error_) {
     if (ociError!=NULL) {
         // this is error: call of virtual function from constructor.
         //*******fillDBError(ociError);
         //  next two strings - body of original fillDBError
         oracle8QueryManager_p_->fillDBError(queryError_,ociError);  
     }
     CORBA::String_var why = queryError_.why;
     GradSoft::Logger::WarningLocker wl(getLogger().warnings());
     if (getDebugLevel() > 5) {
        getLogger().warnings() << "[" << __FILE__ << ":" << __LINE__ << "] ";
     }
     getLogger().warnings() << "error during creation of Oracle query:" << why.in() << std::endl;
     if (handles_p_) {
	     //TODO: free handles_p
       handles_p_ = NULL;
     }
   }
   ociStmt_p_=NULL;
   ociParamsBind_pp_=NULL;
   ociResultDefs_pp_=NULL;
  }

Oracle8Query::~Oracle8Query()
{
 try {
   close();
 }catch(const UAKGQuery2::QueryProcessingError& ex){
   if (getDebugLevel() > 5) {
        getLogger().warnings() << "[" << __FILE__ << ":" << __LINE__ << "] ";
   }
   getLogger().warnings() << "Error during destroy of Qracle query:" << ex.why << std::endl;
 }
}
  
void Oracle8Query::prepare_internal()  
{
#ifdef CORBA_HAVE_OTS
 bool inTransaction = joinCurrentTransaction();
#ifdef USE_OB_XA
 OCIEnv* ociEnv_p = xa_ ? xaoEnv((text*)getDBName_p()) : 
                                                  handles_p_->getEnv_p();
 OCISvcCtx* ociSvcCtx_p = xa_ ? xaoSvcCtx((text*)getDBName_p()) : 
                                                  handles_p_->getSvcCtx_p();
#else
 OCIEnv* ociEnv_p = handles_p_->getEnv_p();
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
#endif
#else
 OCIEnv* ociEnv_p = handles_p_->getEnv_p();
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
#endif
    if (ociStmt_p_ == NULL) {
      try {
        checkError
        (
            OCIHandleAlloc
            (
                ociEnv_p, 
                (dvoid**)&ociStmt_p_,
                OCI_HTYPE_STMT, 
                (size_t)0, 
                (dvoid**)NULL
            )
        );
      }catch(const QueryProcessingError& ){
        ociStmt_p_ = NULL;
        throw;
      }
    }

    checkError
    (
        OCIStmtPrepare
        (
            ociStmt_p_, 
            ociError_p(), 
            (text*)queryText_, 
            strlen(queryText_),
            OCI_NTV_SYNTAX, 
            OCI_DEFAULT
        )
    );

    checkError(OCIAttrGet((dvoid*)ociStmt_p_,(ub4)OCI_HTYPE_STMT,
                       &stmtType_,(ub4 *)NULL,(ub4)OCI_ATTR_STMT_TYPE,
                       ociError_p()
           )          );
    if (stmtType_ == OCI_STMT_SELECT)  {
       selectFlg_ = true;
    }


    RecordSet_var rs = getParamsBindDescription();

    setParamsOCIBinds(rs->getNColumns());


    rebind();

   
    RecordSet_var r_rs = buildResultBindDescription();
    setResultBindDescription(r_rs);
    setResultOCIDefs(r_rs);

}
  
void Oracle8Query::setResultOCIDefs(RecordSet* rs)
{
    CORBA::ULong nDefs = rs->getNColumns();
    freeResultOCIDefs_pp();
    ociResultDefs_pp_ = new OCIDefine*[nDefs];
    for (CORBA::ULong i=0; i< nDefs; ++i)
        ociResultDefs_pp_[i]=NULL;
    defineOCIDefs(rs);
}

void Oracle8Query::defineOCIDefs(RecordSet* rs)
{
    CORBA::ULong nDefs = rs->getNColumns();
    for(CORBA::ULong defCount = 0; defCount < nDefs; ++defCount) 
    {
        

        checkError
        (
            OCIDefineByPos                       
            (
                /* OCIStmt    *stmtp,  */  ociStmt_p_,
                /* OCIDefine  **defnp, */  &ociResultDefs_pp_[defCount],
                /* OCIError   *errhp,  */  ociError_p(),
                /* ub4        position,*/  defCount + 1,
                /* dvoid      *valuep, */  resultBindSet_p_->getColAddr(defCount),
                /* sb4        value_sz,*/  getFieldSize(rs, defCount),
                /* ub2        dty,     */  valueType2externalDType(rs->getFieldTypeAt(defCount)),
                /* dvoid      *indp,   */  resultBindSet_p_->getColNullIndicators(defCount),
                /* ub2        *rlenp,  */  resultBindSet_p_->getColFlens(defCount),
                /* ub2        *rcodep, */  0, 
                /* ub4        mode     */  OCI_DEFAULT
            )
        );

        if (rs->getFieldTypeAt(defCount) == TypeWString) {
            ub1 cform = SQLCS_NCHAR;
#ifndef OCI_UTF16ID
            ub2 csid = OCI_UCS2ID;//OCI_UTF16ID
#else
            ub2 csid = OCI_UTF16ID;
#endif

            checkError(
                OCIAttrSet(
                    (dvoid*) ociResultDefs_pp_[defCount],
                    (ub4) OCI_HTYPE_DEFINE,
                    (dvoid*) &cform,
                    (ub4) 0,
                    (ub4) OCI_ATTR_CHARSET_FORM,
                    ociError_p()
                )
            );

            checkError
            (
                OCIAttrSet
                (
                    (dvoid*) ociResultDefs_pp_[defCount],
                    (ub4) OCI_HTYPE_DEFINE,
                    (dvoid*) &csid,
                    (ub4) 0,
                    (ub4) OCI_ATTR_CHARSET_ID,
                    ociError_p()
                )
            );


        }
    }
}

void Oracle8Query::execute_internal()
{
#ifdef CORBA_HAVE_OTS
 bool inTransaction = joinCurrentTransaction();
#ifdef USE_OB_XA
 OCISvcCtx* ociSvcCtx_p = xa_ ? xaoSvcCtx((text*)getDBName_p()) :
                                                  handles_p_->getSvcCtx_p();
#else
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
#endif
#else
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
#endif

    ub4 ociTransFlag = OCI_DEFAULT;
    if (flags_.get_int("ReadOnly")) {
       ociTransFlag |= OCI_TRANS_READONLY;
    }
#ifndef CORBA_HAVE_OTS
    ociTransFlag |= OCI_COMMIT_ON_SUCCESS;
#else
    if (!inTransaction) {
      ociTransFlag |= OCI_COMMIT_ON_SUCCESS;
    }
#endif

    int paramsNRows = paramsBindSet_p_->getNRows();
    if (paramsNRows==0 || paramsBindSet_p_->getNColumns()==0) {
       paramsNRows = 1;
    }

    rebind();

    sword err;
    err = OCIStmtExecute
    (
        ociSvcCtx_p,
        ociStmt_p_,
        ociError_p(),
        (selectFlg_ ? resultChunkSize_ : paramsNRows),
        0,NULL,NULL,
        ociTransFlag
    );

    if (err != OCI_NO_DATA) checkError(err);
    else retrieved_ = true;
    ub4 rowCount;
    checkError
    (
        OCIAttrGet
        (
            (dvoid*)ociStmt_p_,
            (ub4)OCI_HTYPE_STMT,
            (dvoid*)&rowCount,
            (ub4 *)NULL,
            (ub4)OCI_ATTR_ROW_COUNT,
            ociError_p()
        )
    );
    retrievedNRows_ = rowCount;
    executed_ = true;

}

CORBA::Boolean Oracle8Query::retrieve_internal()
{
#ifdef CORBA_HAVE_OTS
#ifndef USE_OB_XA
    joinCurrentTransaction();
#endif
#endif
    if (fetchedNRows_ < retrievedNRows_) {
       return !retrieved_;  // i. e. we must "physically" call retrieve_internal
                            // only if we fetch all before.
    }
    if (!selectFlg_) {
       return false;
    }
    if (!retrieved_)
    {


        resultBindSet_p_->initialize();

        sword ociResult = OCI_ERROR;
        try {
           ociResult = OCIStmtFetch2
                       (
                        ociStmt_p_,
                        ociError_p(),
                        resultChunkSize_,
                        OCI_FETCH_NEXT,
                        1,
                        OCI_DEFAULT
                       ); 
        }catch(const std::exception& ex){
           if (getDebugLevel()>5) { 
             getLogger().errors() << "[" <<__FILE__<<":"<<__LINE__<< "] ";  
           }
           getLogger().errors() << ex.what() << std::endl;
        }catch(...){
           if (getDebugLevel()>5) { 
             getLogger().errors() << "[" <<__FILE__<<":"<<__LINE__<< "] ";  
           }
           getLogger().errors() << "unknown exception in OCIStmtFetch2" << std::endl;
        }
        if (ociResult == OCI_NO_DATA) {
           retrieved_ = true;
        }else{
           checkError(ociResult);
        }
        checkError
        (
            OCIAttrGet
            (
                (dvoid*)ociStmt_p_, 
                (ub4)OCI_HTYPE_STMT,
                (dvoid*)&retrievedNRows_, 
                (ub4 *)NULL, 
                (ub4)OCI_ATTR_ROW_COUNT,
                ociError_p()            
            )            
        );

    }else{
        return false;
 //           rebind();
 //           execute_rebinded();
 //           return true;
 //      }
    }

    return !retrieved_;
}

RecordSet* Oracle8Query::buildResultBindDescription() 
{
#ifdef USE_OB_XA
 OCISvcCtx* ociSvcCtx_p = xa_ ? xaoSvcCtx((text*)getDBName_p()) : 
                                                  handles_p_->getSvcCtx_p();
#else
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p(); 
#endif

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
    

 checkError(OCIAttrGet((dvoid*)ociStmt_p_,(ub4)OCI_HTYPE_STMT,
                       &stmtType,(ub4 *)NULL,(ub4)OCI_ATTR_STMT_TYPE,
                       ociError_p()
           )          );

 if (stmtType == OCI_STMT_SELECT) 
 {
   selectFlg_ = true;

   checkError(OCIStmtExecute(ociSvcCtx_p,ociStmt_p_,ociError_p(),1,0,
                           NULL,NULL,OCI_DESCRIBE_ONLY));

   checkError(OCIAttrGet((dvoid*)ociStmt_p_,(ub4)OCI_HTYPE_STMT,
                       &paramCount,(ub4 *)NULL,(ub4)OCI_ATTR_PARAM_COUNT,
                       ociError_p()
           )          );

   for(paramCounter=1;paramCounter <= paramCount; ++paramCounter)
   {
     checkError(
                OCIParamGet(ociStmt_p_,OCI_HTYPE_STMT,ociError_p(),
                           (dvoid**)&ociParam_p,paramCounter)
               );

     checkError(
       OCIAttrGet((dvoid*)ociParam_p, (ub4)OCI_DTYPE_PARAM,
                (dvoid*)&dtype, (ub4 *)NULL, (ub4)OCI_ATTR_DATA_TYPE,
                ociError_p()
       )
     );
     checkError(
       OCIAttrGet((dvoid*)ociParam_p, (ub4)OCI_DTYPE_PARAM,
                 (dvoid*)&fieldLength, (ub4 *)NULL, (ub4)OCI_ATTR_DATA_SIZE,
                 ociError_p()
       )
     );
     checkError(
       OCIAttrGet((dvoid*)ociParam_p, (ub4)OCI_DTYPE_PARAM,
                (dvoid*)&columnName, &columnNameLength, 
                (ub4)OCI_ATTR_NAME,
                ociError_p()
       )
     );

     ub2 precision=0;
     sb1 scale=0;
     ub2 csid=0;
     switch(dtype){
      case OCI_TYPECODE_VARCHAR2:
      case OCI_TYPECODE_VARCHAR:
      case OCI_TYPECODE_CHAR:
                checkError(
                    OCIAttrGet((dvoid*)ociParam_p,(ub4)OCI_DTYPE_PARAM,
                        (dvoid*)&csid, (ub4 *)NULL,
                        (ub4)OCI_ATTR_CHARSET_ID,
                        ociError_p()
                    )
                );
                //++fieldLength;     // add one byte for '\0'  
                ///  instedl correct later ith +1 or +2 in depend of 
                //   String/WString
                break;
      case OCI_TYPECODE_DATE:           
                fieldLength = dbFormatHandler_->datetime_length();
                break;
      case OCI_TYPECODE_BLOB:
                fieldLength = sizeof(OCILobLocator*);
                break;
      case OCI_TYPECODE_CLOB:
                if (useStrClob_) {
                  fieldLength = MAX_CLOB_SIZE;
                }else{
                  if (fieldLength < sizeof(OCILobLocator*)) {
                     fieldLength = sizeof(OCILobLocator*);
                  }
                }
                break;
      case OCI_TYPECODE_NUMBER:
      case OCI_TYPECODE_DECIMAL:
        checkError(
            OCIAttrGet((dvoid*)ociParam_p,(ub4)OCI_DTYPE_PARAM,
                  (dvoid*)&precision, (ub4 *)NULL,
                  (ub4)OCI_ATTR_PRECISION,
                  ociError_p()
        )   );
        checkError(
            OCIAttrGet((dvoid*)ociParam_p,(ub4)OCI_DTYPE_PARAM,
                  (dvoid*)&scale, (ub4 *)NULL,
                  (ub4)OCI_ATTR_SCALE,
                  ociError_p()
            )
        );
        break;
      default:
        getLogger().errors() << "Oracle8Query::buildResultBindDescription, unknown type :" << dtype << std::endl; 
     }
     OCIHandleFree( ociParam_p, OCI_HTYPE_DESCRIBE);

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


     FieldType ftype = 
           internalDType2valueType(dtype,fieldLength,precision,scale,csid);

     switch(ftype) {
        case TypeString:
               ++fieldLength;
               break;
        case TypeWString:
               fieldLength+=2;
               break;
        default:
               break;
     }

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

void Oracle8Query::setParamsOCIBinds(CORBA::ULong len)
{
 freeParamsOCIBinds_pp();
 ociParamsBind_pp_ =  new OCIBind*[len];
 for(CORBA::ULong i=0; i<len; ++i) 
     ociParamsBind_pp_[i]=NULL;
}

void Oracle8Query::close()
{
 if (!closed_) {
   freeParamsOCIBinds_pp();
   freeResultOCIDefs_pp();
   UAKGQueryBase::close();
   if (ociStmt_p_!=NULL) {  // checks for ociStmt_p_ & handles_p_ is
          // necessary, because we can fail in constructor, when nothing
          // is allocated yet.
        OCIHandleFree(ociStmt_p_,OCI_HTYPE_STMT);
      ociStmt_p_=NULL;
   }
   if (handles_p_ != NULL) {
      oracle8QueryManager_p_->getSLHndlsPool_p()->dealloc(handles_p_);
      handles_p_ = NULL;
   }
   if (dbFormatHandler_ != NULL) {
       delete dbFormatHandler_;
       dbFormatHandler_ = NULL;
   }
 }
 closed_ = true;
}


void Oracle8Query::setWarning(sword status, const char* msg)
                         
{
  queryError_.dbCode = queryError_.code = status;
  queryError_.why = CORBA::string_dup(msg); 
}

void Oracle8Query::fillDBError()
{
  fillDBError(ociError_p());
}

void Oracle8Query::fillDBError(OCIError* ociError_p)
{
 oracle8QueryManager_p_->fillDBError(queryError_,ociError_p);  
}

void Oracle8Query::checkError(sword status)
{
    switch(status){
       case OCI_SUCCESS:
          queryError_.dbCode = queryError_.code = 0;
          queryError_.why = CORBA::string_dup(""); 
          break;
       case OCI_SUCCESS_WITH_INFO:
          setWarning(status,"warning: OCI_SUCCESS_WITH_INFO");
          break;
       case OCI_NEED_DATA:
          throwQueryProcessingError((CORBA::ULong)3,"Error: OCI_NEED_DATA", false);
          break;
       case OCI_NO_DATA:
          setWarning(status,"warning: OCI_NO_DATA");
          break;
       case OCI_ERROR:
          throwQueryProcessingError(status,"Error: OCI_ERROR",true);
          break;
       case OCI_INVALID_HANDLE:
          throwQueryProcessingError(status,"Error: OCI_INVALID_HANDLE",false);
          break;
       case OCI_STILL_EXECUTING:
          throwQueryProcessingError(status,"Error: OCI_STILL_EXECUTING",false);
          break;
       case OCI_CONTINUE:
          throwQueryProcessingError(status,"Error: OCI_CONTINUE",false);
          break;
       default:
          throwQueryProcessingError(status,"Error: Unknown Error",false);
          break;
    }
}

#ifdef  _MSC_VER
#pragma warning(disable:4715)           // disable not all path return value
#endif  // _MSC_VER

FieldType Oracle8Query::internalDType2valueType
(
    ub2 dtype, 
    ub2& dlen,
    ub2 precision,
    sb1 scale,
    ub2 charsetid
)
{
    switch(dtype){
        case  1:   //VARCHAR2
#ifndef OCI_UTF16ID
            if (charsetid == OCI_UCS2ID) {
                return TypeWString;//OCI_UTF16ID
            }
#else
            if (charsetid == OCI_UTF16ID
                    || charsetid == 2000) // means AL16UTF16 
            {
                return TypeWString;
            }
#endif
            return TypeString;
        case  2:   // NUMBER
            if (scale == 0)
            {
                if (precision == 0) {
                    dlen=dbFormatHandler_->decimal_length(); 
                    return TypeNumeric;
                }
                if (precision < 5) 
               { 
                    dlen = sizeof(CORBA::Short); 
                    return TypeShort;
                }
                if (precision < 10) 
                { 
                    dlen = sizeof(CORBA::Long); 
                    return TypeLong;
                }
                if (precision < 18) 
                {
                    dlen = sizeof(CORBA::LongLong); 
                    return TypeLongLong;
                }
            }
            else
            {
                if (precision <= 6 /* FLT_DIG */)
                {
                    dlen = sizeof(CORBA::Float); 
                    return TypeFloat;
                }
                if (precision <= 15 /* DBL_DIG */) 
                {
                    dlen = sizeof(CORBA::Double); 
                    return TypeDouble;
                }
            }
            dlen=dbFormatHandler_->decimal_length(); 
            return TypeNumeric;
        case 8:  // CORBA::Long
            return TypeClob;
        case 11: // ROWID
            return TypeRaw;
        case 12: // DATE
            dlen = dbFormatHandler_->datetime_length();
            return TypeDateTime;
        case 23: // RAW
            return TypeRaw;
        case 24: // LONG RAW
            return TypeBlob;
        case 96:   //CHAR
#ifndef OCI_UTF16ID
            if (charsetid == OCI_UCS2ID) {
              return TypeWString;//OCI_UTF16ID
            }
#else
            if (charsetid == OCI_UTF16ID
                    || charsetid == 2000) // means AL16UTF16
            {
                    return TypeWString;
            }
#endif
            return TypeString;
        case 69: // ROWID
            return TypeRaw;
        case 105:   //MSLABEL
            return TypeRaw;
/*
        case 108:   //USER-DEFINED
            return TypeObject;
        case 111:   //REF
            return TypeObject;
*/
        case 112:   //CLOB
            if (useStrClob_) {
              return TypeString;
            }else{
              return TypeClob;
            }
        case 113:   //BLOB
            return TypeBlob;
        case 114:   //BFILE
            return TypeBlob;
        case 180: // TIMESTAMP
        case 181:
            dlen = dbFormatHandler_->datetime_length();
            return TypeDateTime;
        default:
            throwQueryProcessingError(2001,"unknown internal datatype",false);
    }
}

#ifdef  _MSC_VER
#pragma warning(default:4715) //not all path return value enabled
#endif  // _MSC_VER


ub2   Oracle8Query::valueType2externalDType(FieldType vt)
{
  switch(vt) {
    case TypeBoolean:
          return SQLT_INT;
    case TypeChar:
          return SQLT_AFC;
    case TypeOctet:
    case TypeShort:
    case TypeUShort:
    case TypeLong:
    case TypeULong:
    case TypeLongLong:
    case TypeULongLong:
          return SQLT_INT;
    case TypeFloat:
    case TypeDouble:
          return SQLT_FLT;
    case TypeWString:
          return SQLT_STR;
    case TypeString:
          return SQLT_STR;
/*
    case TypeObject:
          throwQueryProcessingError(3,"TypeObject is not supported",false);
          return 0;
*/
    case TypeSmallInt:
    case TypeInteger:
          return SQLT_INT;
    case TypeReal:
    case TypeDoublePrecision:
          return SQLT_FLT;
    case TypeCharacter:
          return SQLT_AFC;
    case TypeDecimal:
    case TypeNumeric:
          return SQLT_VNU;
    case TypeRaw:
          return SQLT_VBI;
    case TypeDateTime:
          return SQLT_DAT;
    case TypeBlob:                
          return SQLT_BLOB;
    case TypeClob:
          if (useStrClob_) {
            return SQLT_STR;
          } else {
            return SQLT_CLOB;
          }
    case TypeWclob:
          return SQLT_CLOB;
    default:                                           
          throwQueryProcessingError(1,"Internal Error: unknown ValueType ",false);
          return 0;
  }
}

CORBA::ULong  
Oracle8Query::getFieldSize(UAKGQuery2::RecordSet* rs, CORBA::ULong col) 
{
    if (rs->haveFieldSizeAt(col)) {
        if (rs->getFieldTypeAt(col)==TypeWString) {
          return rs->getFieldSizeAt(col)+2;
        }else if (rs->getFieldTypeAt(col)==TypeString) {
           return rs->getFieldSizeAt(col);
        }else{
           return rs->getFieldSizeAt(col);
        }
    }

    switch ( rs->getFieldTypeAt(col) ) {
      case TypeBoolean:
            return sizeof(CORBA::Boolean);
      case TypeChar:
      case TypeCharacter:
            return sizeof(CORBA::Char);
      case TypeOctet:
            return sizeof(CORBA::Octet);
      case TypeShort:
      case TypeSmallInt:
      case TypeUShort:
            return sizeof(CORBA::Short);
      case TypeLong:
      case TypeInteger:
      case TypeULong:
            return sizeof(CORBA::Long);
      case TypeLongLong:
      case TypeULongLong:
            return sizeof(CORBA::LongLong);
      case TypeFloat:
      case TypeReal:
            return sizeof(CORBA::Float);
      case TypeDouble:
      case TypeDoublePrecision:
            return sizeof(CORBA::Double);
      case TypeDateTime:
            return dbFormatHandler_->datetime_length();
      case TypeBlob:
            return sizeof(OCILobLocator*);
      case TypeClob:
            if (useStrClob_) {
              return MAX_CLOB_SIZE;
            } else {
              return sizeof(OCILobLocator*);
            }
      default:
            getLogger().errors() << "Oracle8Query::getFieldSize - unknown type"
                                 << rs->getFieldTypeAt(col) << std::endl;
    }
    return 0;                                                       
}

CORBA::ULong Oracle8Query::correctFieldSizeForBind(ub2 externalIdType, 
                                     FieldType fdtype,
                                     CORBA::ULong fdsize)
{
 switch(externalIdType) {
   case SQLT_STR:
   case SQLT_AFC:
   case SQLT_AVC:
           return ((fdtype==TypeWString) ? fdsize+2 : fdsize+1 );
   case SQLT_INT:
           switch(fdtype) {
                case TypeOctet:  
                           return 1;
                case TypeShort:
                case TypeUShort:
                           return 2;
                case TypeLong:
                case TypeULong:
                           return 4;
                case TypeLongLong:
                case TypeULongLong:
                           return 8;
                default:
                       if (getDebugLevel() > 5) {
                          getLogger().warnings() << "[" <<  __FILE__ << ":" << __LINE__ << "] ";
                       }
                       getLogger().warnings() <<  "Oracle8Query::correctFieldSizeForBind - unknown fd for SQLY_INT " << externalIdType << std::endl;
                       return fdsize;
            }
   case SQLT_FLT:
           return fdsize;
   case SQLT_BFLOAT:
           return sizeof(float);
   case SQLT_BDOUBLE:
           return sizeof(double);
   case SQLT_VCS:
           return fdsize+sizeof(short);
   case SQLT_VBI:
           return fdsize+sizeof(short);
   case SQLT_LVC:
           return fdsize+sizeof(int);
   case SQLT_CLOB:
           if (useStrClob_) {
             return MAX_CLOB_SIZE;
           }
           // then continue
   case SQLT_BLOB:
   case SQLT_FILE:
           if (fdsize < sizeof(OCILobLocator*)) {
              return sizeof(OCILobLocator*);
           }else{
              return fdsize;
           }
   case SQLT_VNU:
           return getDBFormatHandler_p()->decimal_length();
   case SQLT_DAT:
           return getDBFormatHandler_p()->datetime_length();
   default:
        getLogger().errors() << "Oracle8Query::correctFieldSizeForBind - unknown type " << externalIdType << std::endl;
        return fdsize;
 }
}
                                     


void Oracle8Query::freeOCIBinds_pp(OCIBind**& ociBinds_pp, 
                                   DBBindSet* dbBindSet_p)
{
  if (ociBinds_pp==NULL) return;
  assert(dbBindSet_p!=NULL);
  //bind handles are impicit deallocated, so do not call OCIHandleFree 
  delete[] ociBinds_pp;
  ociBinds_pp = NULL;
}

void Oracle8Query::freeParamsOCIBinds_pp()
{
  freeOCIBinds_pp(ociParamsBind_pp_,paramsBindSet_p_);
}


void Oracle8Query::freeResultOCIDefs_pp()
{
  if (ociResultDefs_pp_==NULL) return;
  //for(CORBA::ULong i=0; i<resultBindSet_p_->getNColumns(); ++i){
  //  OCIHandleFree((dvoid*)ociResultDefs_pp_[i], OCI_HTYPE_DESCRIBE);
  //}
  delete[] ociResultDefs_pp_;
  ociResultDefs_pp_ = NULL;
}


void Oracle8Query::rebind()
{
#ifdef CORBA_HAVE_OTS
 bool inTransaction = joinCurrentTransaction();
#ifdef USE_OB_XA
 OCIEnv* ociEnv_p = xa_ ? xaoEnv((text*)getDBName_p()) : 
                                                  handles_p_->getEnv_p();
 OCISvcCtx* ociSvcCtx_p = xa_ ? xaoSvcCtx((text*)getDBName_p()) : 
                                                  handles_p_->getSvcCtx_p();
#else
 OCIEnv* ociEnv_p = handles_p_->getEnv_p();
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
#endif
#else
 OCIEnv* ociEnv_p = handles_p_->getEnv_p();
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
#endif

    RecordSet_var rs = getParamsBindDescription();

    ub4 nParamsRows = (ub4)paramsBindSet_p_->getNRows();
    if (paramsBindSet_p_->getNColumns()==0) nParamsRows=0;

    for(CORBA::ULong i=0; i<rs->getNColumns(); ++i) {
        CORBA::String_var name;
        FieldType fdtype;
        CORBA::ULong fdsize;
        CORBA::ULong _fdsize;

        try {   
           name = rs->getColumnNameByIndex(i);
           fdtype = rs->getFieldTypeAt(i);
           fdsize = getFieldSize(rs, i);
        } catch (const InvalidColumn& ex) {
           throwQueryProcessingError(400, "Error: Invalid column", false); 
        } catch (const UAKGQuery2::InvalidFieldType& ex) {
           throwQueryProcessingError(400, "Error: Invalid field type", false); 
        }

        ub2 externalIdType=valueType2externalDType(fdtype);

 
        _fdsize = correctFieldSizeForBind(externalIdType,fdtype,fdsize);


        checkError
        (
            OCIBindByName
            (
                ociStmt_p_,
                &ociParamsBind_pp_[i],
                ociError_p(),
                (text*)((const char*)name),
                strlen(name),
                (dvoid*)((char*)paramsBindSet_p_->getMemPtr(0,/*current_param_record_*/ i, _fdsize)),
                _fdsize,
                externalIdType,
                (sb2*)(paramsBindSet_p_->getColNullIndicators(i)),
                (ub2*)(paramsBindSet_p_->getColFlens(i)), // - check
                NULL, //(sb2*)(paramsBindSet_p->getCollNullIndicators(i)),  // array of return codes for PL/SQL
                0, //(selectFlg_) ? 0 : paramsBindSet_p_->getNRows(),                
                NULL,   // TODO: set to pointer of actual nuber of elements.
                OCI_DEFAULT
            )
        );
 
        if ( fdtype == TypeWString) {
            ub2 cform = SQLCS_NCHAR;
            checkError
            (
                OCIAttrSet
                (
                    (dvoid*) ociParamsBind_pp_[i],
                    (ub4) OCI_HTYPE_BIND,
                    (dvoid*) &cform,
                    (ub4) 0,
                    (ub4) OCI_ATTR_CHARSET_FORM,
                    ociError_p()
                )
            );
#ifndef OCI_UTF16ID
            ub2 csid = OCI_UCS2ID;//OCI_UTF16ID
#else
            ub2 csid = OCI_UTF16ID;
#endif
            checkError
            (
                OCIAttrSet
                (
                    (dvoid*) ociParamsBind_pp_[i],
                    (ub4) OCI_HTYPE_BIND,
                    (dvoid*) &csid,
                    (ub4) 0,
                    (ub4) OCI_ATTR_CHARSET_ID,
                    ociError_p()
                )
            );
            sb2 wstr_buff_len = (fdsize-2)*2;
            checkError
            (
                OCIAttrSet(
                    (void *) ociParamsBind_pp_[i],
                    (ub4) OCI_HTYPE_BIND,
                    (void *) &wstr_buff_len,
                    (ub4) 0,
                    (ub4)OCI_ATTR_MAXDATA_SIZE,
                    ociError_p()
                )
            );
        }
    }
    /*current_param_record_++*/;
}

void Oracle8Query::execute_rebinded()
{
    if(!selectFlg_) return;

    executed_=false;
    closed_=false;
    retrieved_=false;
    status_ = incomplete;
    retrievedNRows_=0;
    fetchedNRows_=0;
    resultRowIndex_=0;
    error_=false;


#ifdef CORBA_HAVE_OTS
// bool inTransaction = joinCurrentTransaction();
#ifdef USE_OB_XA
 OCISvcCtx* ociSvcCtx_p = xa_ ? xaoSvcCtx((text*)getDBName_p()) :
                                                  handles_p_->getSvcCtx_p();
#else
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
#endif
#else
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
#endif

    ub4 ociTransFlag = OCI_DEFAULT;
    if (flags_.get_int("ReadOnly")) {
       ociTransFlag |= OCI_TRANS_READONLY;
    }
#ifndef CORBA_HAVE_OTS
    ociTransFlag |= OCI_COMMIT_ON_SUCCESS;
#else
    if (!inTransaction) {
      ociTransFlag |= OCI_COMMIT_ON_SUCCESS;
    }
#endif


    sword err;
    err = OCIStmtExecute
    (
        ociSvcCtx_p,
        ociStmt_p_,
        ociError_p(),
        resultChunkSize_,
        0,NULL,NULL,
        ociTransFlag
    );

    if (err != OCI_NO_DATA) checkError(err);
    else retrieved_ = true;


    ub4 rowCount;
    checkError
    (
        OCIAttrGet
        (
            (dvoid*)ociStmt_p_,
            (ub4)OCI_HTYPE_STMT,
            (dvoid*)&rowCount,
            (ub4 *)NULL,
            (ub4)OCI_ATTR_ROW_COUNT,
            ociError_p()
        )
    );
    retrievedNRows_ = rowCount;
    fetchedNRows_ = 0;
    resultRowIndex_ = 0;
    executed_=true;
}


PortableServer::ObjectId* Oracle8Query::createTemporaryLobId(ub1 lob_type)
{
// TODO:
//   recheck with transaction.
	OCIEnv* ociEnv_p = handles_p_->getEnv_p();
	OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();

    OCILobLocator* lob;
    checkError(
        OCIDescriptorAlloc(
                ociEnv_p, 
                (dvoid**)&lob,
                OCI_DTYPE_LOB,
                (size_t)0, 
                (dvoid**)NULL)
    );

    checkError(
        OCILobCreateTemporary(ociSvcCtx_p,
                ociError_p(),
                lob,
                (ub2)0, 
                SQLCS_IMPLICIT, 
                lob_type, 
                OCI_ATTR_NOCACHE, 
                OCI_DURATION_SESSION)
    );

	return createLobId(lob);
}

PortableServer::ObjectId* Oracle8Query::createLobId(OCILobLocator* lobData)
{	
  PortableServer::ObjectId_var oid=new PortableServer::ObjectId();
  oid->length(sizeof(OCILobLocator*));
  memcpy(oid->get_buffer(),&lobData,sizeof(OCILobLocator*));
  return oid._retn();
}

void Oracle8Query::assignBlob(void** dstLobDataPtr,void* srcLobData) throw (UAKGQuery2::QueryProcessingError)
{
 OCILobLocator** dstLobLocatorPtr=(OCILobLocator**)dstLobDataPtr; 
 OCILobLocator* srcLobLocator=(OCILobLocator*)srcLobData; 
 if (srcLobLocator==*dstLobLocatorPtr) return; 
 OCISvcCtx* ociSvcCtx_p = handles_p_->getSvcCtx_p();
 checkError(
    OCILobLocatorAssign(ociSvcCtx_p,ociError_p(),srcLobLocator,dstLobLocatorPtr)
           );
}


