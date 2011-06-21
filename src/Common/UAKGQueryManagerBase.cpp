/**
 * part of GradSoft UAKGQuery
 * http://www.gradsoft.com.ua
 * (C) Ruslan Shevchenko, 1998-2001
 * (C) GradSoft 2000 - 2003
 **/
#ifndef __Common_UAKGQueryManagerBase_h
#include <Common/UAKGQueryManagerBase.h>
#endif

#ifndef __Common_UAKGQueryBase_h
#include <Common/UAKGQueryBase.h>
#endif

#ifndef __Common_UAKGQueryDestroyGuard_h
#include <Common/UAKGQueryDestroyGuard.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif


#include <sstream>
#include <string>
using namespace std;
using namespace GradSoft;
using namespace UAKGQuery2;

UAKGQueryManagerBase::UAKGQueryManagerBase() 
  :dbLink_(NULL),
   username_(NULL),
   password_(NULL),
   szFlags_(NULL),
   flags_(),
   properties_(),
   nQueries_(0),
   nQueriesMutex_(),
   dbConnectionManagers_p_(NULL),
   lastTouch_(time(NULL)),
   blobWritingPOA_(PortableServer::POA::_nil()),
   clobWritingPOA_(PortableServer::POA::_nil()),
   wclobWritingPOA_(PortableServer::POA::_nil()),
   blobReadingPOA_(PortableServer::POA::_nil()),
   clobReadingPOA_(PortableServer::POA::_nil()),
   wclobReadingPOA_(PortableServer::POA::_nil()),
   blobWritingServantPtr_(NULL),
   clobWritingServantPtr_(NULL),
   wclobWritingServantPtr_(NULL),
   blobReadingServantPtr_(NULL),
   clobReadingServantPtr_(NULL),
   wclobReadingServantPtr_(NULL),
   isDestroyed_(false)
{
}

UAKGQueryManagerBase::~UAKGQueryManagerBase()
{
  if (!CORBA::is_nil(blobWritingPOA_.in()))
      blobWritingPOA_->destroy(false, false);
  if (!CORBA::is_nil(clobWritingPOA_.in()))
      clobWritingPOA_->destroy(false, false);
  if (!CORBA::is_nil(wclobWritingPOA_.in()))
      wclobWritingPOA_->destroy(false, false);
  if (!CORBA::is_nil(blobReadingPOA_.in()))
      blobReadingPOA_->destroy(false, false);
  if (!CORBA::is_nil(clobReadingPOA_.in()))
      clobReadingPOA_->destroy(false, false);
  if (!CORBA::is_nil(wclobReadingPOA_.in()))
      wclobReadingPOA_->destroy(false, false);
  if (blobWritingServantPtr_) {
	  delete blobWritingServantPtr_;
  }
  if (clobWritingServantPtr_) {
	  delete clobWritingServantPtr_;
  }
  if (wclobWritingServantPtr_) {
	  delete wclobWritingServantPtr_;
  }
  if (blobReadingServantPtr_) {
	  delete blobReadingServantPtr_;
  }
  if (clobReadingServantPtr_) {
	  delete clobReadingServantPtr_;
  }
  if (wclobReadingServantPtr_) {
	  delete wclobReadingServantPtr_;
  }
  if (dbLink_) delete[] dbLink_;
  if (username_) delete[] username_;
  if (password_) delete[] password_;
  if (szFlags_) delete[] szFlags_;
}

CORBA::ORB_ptr UAKGQueryManagerBase::getORB_()
{
 return dbConnectionManagers_p_->getORB_();
}

PortableServer::POA_ptr UAKGQueryManagerBase::getPOA_()
{
 return dbConnectionManagers_p_->getConnectionsPOA_();
}

PortableServer::POA_ptr UAKGQueryManagerBase::_default_POA()
{
 return dbConnectionManagers_p_->getConnectionsPOA();
}

PortableServer::POA_ptr UAKGQueryManagerBase::getQueriesPOA_()
{
 return dbConnectionManagers_p_->getQueriesPOA_();
}

const char* UAKGQueryManagerBase::get_sz_flag(const char* flagName) const
{
 return flags_.get_sz(flagName);
}

int UAKGQueryManagerBase::get_int_flag(const char* flagName) const
{
 return flags_.get_int(flagName);
}


void UAKGQueryManagerBase::destroy() throw (CORBA::SystemException)
{
 try {
  getLogger().infos() << "destroy query" << std::endl;
  PortableServer::ObjectId_var oid; 
  try {
    oid = getPOA_()->servant_to_id(this);
  }catch(const CORBA::SystemException& ex) {
    GradSoft::Logger::WarningLocker l(getLogger().warnings());
    getLogger().warnings() << "System exception during call servant_to_id:"
                          << ex << std::endl;
    throw; 
  }catch(const PortableServer::POA::WrongPolicy& ex) {
    GradSoft::Logger::WarningLocker l(getLogger().warnings());
    getLogger().warnings() << "POA::WrongPolicy " 
                           << std::endl;
    throw std::runtime_error("internal error: wrong POA Policy during call of reference_to_id");
  }catch(const PortableServer::POA::ServantNotActive& ex) {
    GradSoft::Logger::WarningLocker l(getLogger().warnings());
    getLogger().warnings() << "POA::ServantNotActive " 
                           << std::endl;
    throw std::runtime_error("internal error: ServantNotActive during call of reference_to_id");
  }catch(const std::exception& ex){
    getLogger().warnings() << "can't transform servant to id:" << ex.what() << std::endl;
  }catch(...){
    getLogger().warnings() << "can't transform servant to id (...)"  << std::endl;
    throw;
  }
  getLogger().infos() << "receive oid" << std::endl;
  CORBA::String_var soid=PortableServer::ObjectId_to_string(oid.in());
  getLogger().infos() << "receive soid:" << soid.in() << std::endl;
  DBConnectionManagers_impl::ConnectionRecord* crp=NULL;
  long id=-1;
  try {
    id=dbConnectionManagers_p_->getConnectionRecordId(soid.in());
  }catch(const std::invalid_argument& ex){
    GradSoft::Logger::WarningLocker l(getLogger().warnings());
    if (getDebugLevel()>5) {
       getLogger().warnings() <<"["<<__FILE__<<":"<<__LINE__<<"]";
    }
    getLogger().warnings() << "impossible, destroy of unexisten oid:" 
	                   << soid << std::endl;
    throw CORBA::OBJECT_NOT_EXIST();
  }catch(const std::exception& ex){
    GradSoft::Logger::WarningLocker l(getLogger().warnings());
    if (getDebugLevel()>5) {
       getLogger().warnings() <<"["<<__FILE__<<":"<<__LINE__<<"]";
    }
    getLogger().warnings() << "error during connecton search:"  << ex.what() << std::endl;
  }catch(...) {
     getLogger().errors() << "unhandled exception during destroy of unexistend oid" << std::endl;
  }
  getLogger().infos() << "try to find connection record with id " << id << std::endl;
  try {
    crp=dbConnectionManagers_p_->findConnectionRecordPtr(id);
  }catch(const DBConnectionManagers_impl::ConnectionRecordNotFound& ex){
    GradSoft::Logger::WarningLocker l(getLogger().warnings());
    if (getDebugLevel()>5) {
       getLogger().warnings() <<"["<<__FILE__<<":"<<__LINE__<<"]";
    }
    getLogger().warnings() << "destroy of connection oid without record:" 
	                   << soid << std::endl;
    throw CORBA::OBJECT_NOT_EXIST();
  }catch(const std::exception& ex){
    GradSoft::Logger::WarningLocker l(getLogger().warnings());
    if (getDebugLevel()>5) {
       getLogger().warnings() <<"["<<__FILE__<<":"<<__LINE__<<"]";
    }
    getLogger().warnings() << "exception durting find connection record:" << ex.what() << std::endl;
  }
  isDestroyed_=true;
  try {
     getLogger().infos() << "deactivate object " << std::endl;
     getPOA_()->deactivate_object(oid.in());
     getLogger().infos() << "done " << std::endl;
  }catch(const CORBA::SystemException& ex) {
     GradSoft::Logger::WarningLocker l(getLogger().warnings());
     if (getDebugLevel()>5) {
       getLogger().warnings() <<"["<<__FILE__<<":"<<__LINE__<<"]";
     }
     getLogger().warnings() << "internal error during object deactivation:" 
	                    << id << std::endl;
     getLogger().warnings() << ex << std::endl;
  }catch(const::std::exception& ex) {
     GradSoft::Logger::WarningLocker l(getLogger().warnings());
     if (getDebugLevel()>5) {
       getLogger().warnings() <<"["<<__FILE__<<":"<<__LINE__<<"]";
     }
     getLogger().warnings() << "internal error during object deactivation:" 
	                    << id << std::endl;
     getLogger().warnings() << ex.what() << std::endl;
  }
  // moved to DBConnectionsServantActivator::etherealize
  //dbConnectionManagers_p_->removeQMReference(id);
 }catch(const CORBA::SystemException& ex) {
   // rethrow: will be passed to called.
   throw; 
 }catch(...){
   //
   getLogger().errors() << "unhandled exception during destroy" << std::endl;
   // no addition information for now.
 }
}


#ifdef FUNCTION
#undef FUNCTION
#endif
#define FUNCTION  "UAKGQueryManagerBase::init"
void  UAKGQueryManagerBase::init( 
                    DBConnectionManagers_impl* dbConnectionManagers_p,
		    const DBConnectionParams* params_p)
{
 dbConnectionManagers_p_ = dbConnectionManagers_p;
 username_ = new char[strlen(params_p->getLogin())+1];  
 strcpy(username_,params_p->getLogin());
 password_ = new char[strlen(params_p->getPassword())+1];  
 strcpy(password_,params_p->getPassword());
 dbLink_ = new char[strlen(params_p->getDBName())+1];  
 strcpy(dbLink_,params_p->getDBName());
 szFlags_ = new char[strlen(params_p->getOther())+1];
 strcpy(szFlags_,params_p->getOther());
 try {
   flags_.parse(szFlags_);
 }catch(const StringFlagSet::ParseError& ex){
   std::ostringstream ostr;
   ostr << "bad flags:" << ex.why << '\0';
   const char* str = ostr.str().c_str();
   {
    Logger::ErrorLocker guard(getLogger().errors());
    if (getDebugLevel()>5) {
       getLogger().errors() <<"["<<__FILE__<<":"<<__LINE__<<"]";
    }
    getLogger().errors() << "QueryInvalid: " << str << endl;
   }
   throw QueryFlagInvalid(str);
 }
 dbName_p_ = flags_.get_sz("DB");

 lastTouch_ = params_p->getLastTouch();

#ifdef CORBA_HAVE_OTS
 transCurrent_.out() = CosTransactions::Current::_narrow(
   CORBA::Object_var(
     getORB_()->resolve_initial_references("TransactionCurrent")));
#endif
}
#undef FUNCTION


UAKGQuery2::Query_ptr UAKGQueryManagerBase::create_query(
		            const char* query_text, const char* flags)
	  throw (CORBA::SystemException,
		 UAKGQuery2::QueryFlagInvalid)
{
 touch();
 UAKGQueryBase* query_impl = createUAKGQuery(query_text,flags);
 UAKGQuery2::Query_var query = query_impl->_this();
 query_impl->_remove_ref();
 return query._retn(); 
}

RecordSet* UAKGQueryManagerBase::evaluate( const char* query_text,
					   const char* query_flags,
                     			   RecordSet* rs)
          throw (CORBA::SystemException,
                 UAKGQuery2::QueryFlagInvalid,
                 UAKGQuery2::QueryProcessingError)
{
 touch();
 UAKGQueryBase* qr = this->createUAKGQuery(query_text,query_flags);
 RecordSet_var retval;
 {
  UAKGQueryDestroyGuard qd(qr);
  qr->prepare(rs);
  qr->execute(rs);
  CORBA::Boolean b;
  retval = qr->fetch(0,b);
 }
 return retval._retn(); 
}

RecordSet* UAKGQueryManagerBase::evaluate_e( const char* query_text,
                       			     const char* query_flags)
          throw (CORBA::SystemException,
                 UAKGQuery2::QueryFlagInvalid,
                 UAKGQuery2::QueryProcessingError)
{
 touch();
 UAKGQueryBase* qr = createUAKGQuery(query_text,query_flags);
 RecordSet_var retval;
 {
  UAKGQueryDestroyGuard qd(qr);
  qr->execute_e();
  CORBA::Boolean b;
  retval = qr->fetch(0,b);
 }
 return retval._retn();
}

#if defined(MICO) && MICO_BIN_VERSION < 0x02030b
RecordSet* UAKGQueryManagerBase::evaluate_inout( const char* query_text,
                           			 const char* query_flags,
                           			 RecordSet* rs)
#else
RecordSet* UAKGQueryManagerBase::evaluate_inout( const char* query_text,
                           			 const char* query_flags,
                           			 RecordSet*& rs)
#endif
          throw (CORBA::SystemException,
                 UAKGQuery2::QueryFlagInvalid,
                 UAKGQuery2::QueryProcessingError){
 touch();
 UAKGQueryBase* qr = createUAKGQuery(query_text,query_flags);
 {
  UAKGQueryDestroyGuard qd(qr);
  qr->prepare(rs);
  qr->execute_inout(rs);
  CORBA::Boolean b;
  RecordSet_var retval = qr->fetch(0,b);
  return retval._retn();
 }
}

//
// Utils:
//

Logger&   UAKGQueryManagerBase::getLogger()  const
     { return dbConnectionManagers_p_->getLogger(); }

bool      UAKGQueryManagerBase::logSQL() const  
    { return dbConnectionManagers_p_->logSQL(); }


bool     UAKGQueryManagerBase::getDebugLevel() const
    { return dbConnectionManagers_p_->getDebugLevel(); }

char* UAKGQueryManagerBase::checkStr(char* x) const
{
 if (!x) {
   ErrorLogStream& errs = (const_cast<UAKGQueryManagerBase*>(this))->errors();
   Logger::ErrorLocker guard(errs);
   if (getDebugLevel() > 5) {
       getLogger().errors() <<"["<<__FILE__<<":"<<__LINE__<<"]";
   }
   errs << 
      "Internal Error: attempt to use uninitialized Query Manager " << endl;
//// 
// TODO: Put right parameters in QueryProcessingError instead of 0,0.
///
   throw QueryProcessingError("QueryManager not yet initialized",0,0);
 }
 return x;
}

char* UAKGQueryManagerBase::get_property(const char* property_name)
          throw (CORBA::SystemException,
                 UAKGQuery2::PropertyNotDefined)
{
  const char* retval = properties_.get_sz(property_name);
  if (retval == NULL)
  {
    throw PropertyNotDefined();
  }else{
    return CORBA::string_dup(retval);
  }
}

PortableServer::POA_ptr UAKGQueryManagerBase::createLobPOA_(
          const char* namePrefix, UAKGLobBase* serv)
{
    CORBA::PolicyList p;
    p.length(5);
    PortableServer::POA_ptr poa = getPOA_();
    p[0] = poa->create_id_assignment_policy(PortableServer::USER_ID);
    p[1] = poa->create_servant_retention_policy(PortableServer::NON_RETAIN);
    p[2] = poa->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
    p[3] = poa->create_lifespan_policy(PortableServer::TRANSIENT);
    p[4] = poa->create_request_processing_policy(
            PortableServer::USE_DEFAULT_SERVANT);
    std::ostringstream poaName;
    poaName << namePrefix << this ;
    PortableServer::POA_ptr lobPoa = poa->create_POA(poaName.str().c_str(),
                                        PortableServer::POAManager::_nil(),p);
    lobPoa->set_servant(serv);
    serv->set_POA(lobPoa);
    PortableServer::POAManager_var pm = lobPoa->the_POAManager();
    pm->activate();
	for (CORBA::ULong i = 0; i < p.length(); ++i) p[i]->destroy();
    return lobPoa;
}


PortableServer::POA_ptr UAKGQueryManagerBase::getBlobWritingPOA_()
{
 if (CORBA::is_nil(blobWritingPOA_.in())) {
     UAKGLobBase* serv = createBlobWritingServant();
     blobWritingServantPtr_ = serv;
     blobWritingPOA_ = createLobPOA_("uakg_query_blob_writing_", serv);
 }
 return blobWritingPOA_;
}

PortableServer::POA_ptr UAKGQueryManagerBase::getBlobWritingPOA()
{
 return PortableServer::POA::_duplicate(getBlobWritingPOA_());
}

PortableServer::POA_ptr UAKGQueryManagerBase::getClobWritingPOA_()
{
 if (CORBA::is_nil(clobWritingPOA_.in())) {
     UAKGLobBase* serv = createClobWritingServant();
     clobWritingServantPtr_ = serv;
     clobWritingPOA_ = createLobPOA_("uakg_query_clob_writing_", serv);
 }
 return clobWritingPOA_;
}

PortableServer::POA_ptr UAKGQueryManagerBase::getClobWritingPOA()
{
 return PortableServer::POA::_duplicate(getClobWritingPOA_());
}

PortableServer::POA_ptr UAKGQueryManagerBase::getWclobWritingPOA_()
{
 if (CORBA::is_nil(wclobWritingPOA_.in())) {
     UAKGLobBase* serv = createWclobWritingServant();
     wclobWritingServantPtr_ = serv;
     wclobWritingPOA_ = createLobPOA_("uakg_query_wclob_writing_", serv);
 }
 return wclobWritingPOA_;
}

PortableServer::POA_ptr UAKGQueryManagerBase::getWclobWritingPOA()
{
 return PortableServer::POA::_duplicate(getWclobWritingPOA_());
}

PortableServer::POA_ptr UAKGQueryManagerBase::getBlobReadingPOA_()
{
 if (CORBA::is_nil(blobReadingPOA_.in())) {
     UAKGLobBase* serv = createBlobReadingServant();
     blobReadingServantPtr_ = serv;
     blobReadingPOA_ = createLobPOA_("uakg_query_blob_reading_", serv);
 }
 return blobReadingPOA_;
}

PortableServer::POA_ptr UAKGQueryManagerBase::getBlobReadingPOA()
{
 return PortableServer::POA::_duplicate(getBlobReadingPOA_());
}

PortableServer::POA_ptr UAKGQueryManagerBase::getClobReadingPOA_()
{
 if (CORBA::is_nil(clobReadingPOA_.in())) {
     UAKGLobBase* serv = createClobReadingServant();
     clobReadingServantPtr_ = serv;
     clobReadingPOA_ = createLobPOA_("uakg_query_clob_reading_", serv);
 }
 return clobReadingPOA_;
}

PortableServer::POA_ptr UAKGQueryManagerBase::getClobReadingPOA()
{
 return PortableServer::POA::_duplicate(getClobReadingPOA_());
}

PortableServer::POA_ptr UAKGQueryManagerBase::getWclobReadingPOA_()
{
 if (CORBA::is_nil(wclobReadingPOA_.in())) {
     UAKGLobBase* serv = createWclobReadingServant();
     wclobReadingServantPtr_ = serv;
     wclobReadingPOA_ = createLobPOA_("uakg_query_wclob_reading_", serv);
 }
 return wclobReadingPOA_;
}

PortableServer::POA_ptr UAKGQueryManagerBase::getWclobReadingPOA()
{
 return PortableServer::POA::_duplicate(getWclobReadingPOA_());
}
