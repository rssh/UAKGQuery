#include <Oracle8/Oracle8QueryManager.h>
#include <Oracle8/Oracle8Query.h>

#include <Oracle8/Oracle8Lobs.h>
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


Oracle8QueryManager::Oracle8QueryManager()
   :hndls_p_(NULL), QMWideHandlersSetPtr_(NULL)
{
}

Oracle8QueryManager::~Oracle8QueryManager()
{
 if (is_open() ) {
    close();
 }
}

void Oracle8QueryManager::init( 
                             DBConnectionManagers_impl* dbConnectionManagers_p,
			     const DBConnectionParams* params_p)
{
 UAKGQueryManagerBase::init(dbConnectionManagers_p, params_p);
 hndls_p_ = NULL;
 try {
   hndls_p_ = new SessionLevelOCIHandlersPool(this);
 }catch(const std::runtime_error& ex){
   if (getDebugLevel() > 5) {
     errors() << "[" << __FILE__ << ":" << __LINE__ << "] ";
   }
   errors() << ex.what() << std::endl;
 }
 if (flags_.get_sz("XA")!=NULL) {
   if (strcmp(params_p->getLogin(),"")!=0) {
      errors() << "warning: username ignored in XA connection" << endl;
   }
   if (strcmp(params_p->getPassword(),"")!=0) {
      errors() << "warning: password ignored in XA connection" << endl;
   }
 }
 try {
    properties_.parse("collection=yes");
 }catch(const StringFlagSet::ParseError& ex){
    if (getDebugLevel() > 5) {
     errors() << "[" << __FILE__ << ":" << __LINE__ << "] ";
    }
    errors() << ex.why << std::endl;
    throw QueryFlagInvalid("Can't parse properties string.");
 }
 try {
   QMWideHandlersSetPtr_ = hndls_p_->alloc(NEW_CONTEXT_ALLOC_MODE);
 }catch(const AllocHandleException& ex){
   throwQueryProcessingError(ex.code,"can't alloc handle",true,ex.ociError);
 }catch(const SessionBeginException& ex){
   throwQueryProcessingError(ex.code,"can't begin session",true,ex.ociError);
 }
}

CORBA::Long Oracle8QueryManager::get_next_sequence_value_internal(const char* sequence)
          	throw (  UAKGQuery2::QueryProcessingError,
            	     UAKGQuery2::QueryFlagInvalid,
                	 CORBA::SystemException)
{
    std::ostringstream sql;
    sql << "select " << sequence << ".nextval from dual" << '\0';
    RecordSet_var rs = evaluate_e(sql.str().c_str(), "");
    CORBA::String_var tmp = rs->getAsStringAt(0,0,"0");
    long retval = atoi(tmp);
    return retval;
}
 
void Oracle8QueryManager::close()
{
 if (QMWideHandlersSetPtr_!=NULL) {
    hndls_p_->dealloc(QMWideHandlersSetPtr_);
    QMWideHandlersSetPtr_=NULL;
 }
 delete hndls_p_;
 hndls_p_ = NULL;
}

UAKGQueryBase*  Oracle8QueryManager::createUAKGQuery(
                           const char* query_text, const char* flags)
		          throw(UAKGQuery2::QueryFlagInvalid,
	                  CORBA::SystemException)
{
  return new Oracle8Query(query_text,this,flags);
}

/**
 *  return sql adapted for custom driver
**/
char*  Oracle8QueryManager::normalize_sql(const char* sql)
{
  return CORBA::string_dup(sql);
}


UAKGLobBase* Oracle8QueryManager::createBlobReadingServant()
{
 UAKGLobBase* retval = new Oracle8BlobForReading();
 retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* Oracle8QueryManager::createClobReadingServant()
{
 UAKGLobBase* retval= new Oracle8ClobForReading();
 retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* Oracle8QueryManager::createWclobReadingServant()
{
  UAKGLobBase* retval = new Oracle8WclobForReading();
  retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* Oracle8QueryManager::createBlobWritingServant()
{
 UAKGLobBase* retval = new Oracle8BlobForWriting();
 retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* Oracle8QueryManager::createClobWritingServant()
{
 UAKGLobBase* retval = new Oracle8ClobForWriting();
 retval->setOwnerPtr(this);
 return retval;
}

UAKGLobBase* Oracle8QueryManager::createWclobWritingServant()
{
 UAKGLobBase* retval = new Oracle8WclobForWriting();
 retval->setOwnerPtr(this);
 return retval;
}


void Oracle8QueryManager::checkError(sword status)
{
    switch(status){
       case OCI_SUCCESS:
          break;
       case OCI_SUCCESS_WITH_INFO:
          break;
       case OCI_NEED_DATA:
          throw QueryProcessingError("Error: OCI_NEED_DATA", 3, true);
          break;
       case OCI_NO_DATA:
          throw QueryProcessingError("Error: OCI_NO_DATA", 4, true);
          break;
       case OCI_ERROR:
          throwQueryProcessingError(status,"Error: OCI_ERROR",true);
          break;
       case OCI_INVALID_HANDLE:
          throwQueryProcessingError(status,"Error: OCI_INVALID_HANDLE",true);
          break;
       case OCI_STILL_EXECUTING:
          throwQueryProcessingError(status,"Error: OCI_STILL_EXECUTING",true);
          break;
       case OCI_CONTINUE:
          throwQueryProcessingError(status,"Error: OCI_CONTINUE",false);
          break;
       default:
          throwQueryProcessingError(status,"Error: Unknown Error",false);
          break;
    }
}

void Oracle8QueryManager::fillDBError(QueryProcessingError& queryError, OCIError* ociError_p)
{
  text errbuff[513];
  sb4  errcode = 0;
  memset(errbuff,0,512);
  (void)OCIErrorGet((dvoid*)ociError_p, (ub4)1, (text*)NULL,
                     &errcode, errbuff, (ub4)sizeof(errbuff),
                     OCI_HTYPE_ERROR);
  queryError.dbCode=errcode;
  queryError.why=CORBA::string_dup((char*)errbuff);
  queryError.code=errcode;
}

void Oracle8QueryManager::throwQueryProcessingError(CORBA::ULong errorCode,const char* msg,bool dbFlg, OCIError* ociError_p)
{
 QueryProcessingError ex;
 if (dbFlg) {
   if (hndls_p_==NULL) {
     ex.why = CORBA::string_dup(msg);     
     ex.dbCode=-1;
   }else {
      if (QMWideHandlersSetPtr_!=NULL) { 
	 if (ociError_p==NULL) {
           fillDBError(ex,getOCIError_p());
	 }else{
           fillDBError(ex,ociError_p);
	 }
     }else{
        if (ociError_p!=NULL) {
           fillDBError(ex,ociError_p);
	}else{
          ex.why = CORBA::string_dup(msg);     
          ex.dbCode=-1;
	}
     }
   }
 }else{
   ex.why=CORBA::string_dup(msg);
   ex.dbCode=0;
 }
 {
  GradSoft::Logger::ErrorLocker guard(errors());
  if (getDebugLevel() > 5 ) {
     errors() << "[" << __FILE__ << ":" << __LINE__ << "]" << std::endl;
  }
  errors() << "QueryProcessingError: " <<
               errorCode << "," << ex.dbCode << "\n";
  errors() << "msg: " <<  msg << "\n";
  errors() << "non-sql operation "  << std::endl;
 }
 throw ex;
}

