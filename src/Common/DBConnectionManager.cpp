#include <Common/DBConnectionManager.h>
#include <Common/DBConnectionServantActivator.h>

/*
 * part of GradSoft UAKGQuery
 * (C) GradSoft 1998, 1999, 2000, 2001, 2002, 2003
 * http://www.gradsoft.com.ua
 * $Id: DBConnectionManager.cpp,v 1.40 2007-12-26 13:44:56 rssh Exp $
 */

#include <sstream>


void DBConnectionManagerBase::init(
		GradSoft::ProgOptions& progOptions, 
		GradSoft::Logger& logger,
                PortableServer::POA_ptr myPOA)
{
 progOptions_p_ = &progOptions;
 logger_p_ =  &logger;
 poa_ = PortableServer::POA::_duplicate(myPOA);
}



bool DBDriverParamsLess::operator()(const DBDriverParams& x, const DBDriverParams& y) const
{
 return strcmp(x.driverName_,y.driverName_) < 0 ;
}


DBDriverParams::DBDriverParams(const char* driverName,bool allocate)
{
 if (allocate) {
   driverName_=new char[strlen(driverName)+1];
   strcpy(driverName_,driverName);
 }else{
   driverName_=const_cast<char*>(driverName);
 }
 allocated_ = allocate;
}

DBDriverParams::DBDriverParams(const DBDriverParams& x)
{
  driverName_=new char[strlen(x.driverName_)+1];
  strcpy(driverName_,x.driverName_);
  allocated_ = true;
}

DBDriverParams& DBDriverParams::operator=(const DBDriverParams& x)
{
 if (this != &x) {
   if (allocated_ && driverName_ !=NULL ) {
     delete[] driverName_;
   }
   driverName_=new char[strlen(x.driverName_)+1];
   strcpy(driverName_,x.driverName_);
   allocated_ = true;
 }
 return *this;
}

DBDriverParams::~DBDriverParams()
{
 if (allocated_) {
    delete[] driverName_;
 }
}


DBConnectionManagers_impl::DBConnectionManagers_impl(CORBA::ORB_ptr orb, 
                                PortableServer::POA_ptr systemPOA,
                                PortableServer::POA_ptr userPOA,
				GradSoft::ProgOptions& progOptions,
				GradSoft::Logger&      logger
                                ) 
: driversStorage_(), 
  connectionsStorage_(),
  connectionsStorageMutex_(),
  connectionsCurrentId_(0),
  connectionsBackupFname_("cbUAKGQuery"),
  systemPOA_(PortableServer::POA::_duplicate(systemPOA)),
  myPOA_(PortableServer::POA::_duplicate(userPOA)),
  orb_(CORBA::ORB::_duplicate(orb)),
  shutdowned_(0),
  progOptions_(progOptions),
  logger_(logger),
  cbKey_("a45bcdefgh"),
  nurser_p_(NULL),
  debugLevel_(0)
{
 logSQL_ = progOptions.is_set("log-sql");
 if (progOptions.is_set("cb-key")) {
   cbKey_=progOptions.argument("cb-key");
 }
 if (progOptions.is_set("max-idle-connection-age")) {
    const char* szMaxIdleConnectionAge=
	                  progOptions.argument("max-idle-connection-age");
    maxIdleConnectionAge_=atol(szMaxIdleConnectionAge);
    if (maxIdleConnectionAge_==0) {
        if (getDebugLevel()>5) {
           getLogger().errors() << "[" << __FILE__ << ":" << __LINE__ << "]";
        }
        getLogger().errors() << "Null value of max-idle-connection-age, use 7200" << std::endl;
        maxIdleConnectionAge_=7200;
    }
 }else{
    maxIdleConnectionAge_=7200;
 }
 if (progOptions.is_set("max-idle-persistent-connection-age")) {
    const char* szMaxIdlePersistentConnectionAge=
	           progOptions.argument("max-idle-persistent-connection-age");
    maxIdlePersistentConnectionAge_=atol(szMaxIdlePersistentConnectionAge);
    if (maxIdlePersistentConnectionAge_==0) {
        if (getDebugLevel()>5) {
           getLogger().errors() << "[" << __FILE__ << ":" << __LINE__ << "]";
        }
        getLogger().errors() << "Null value of max-idle-persisten-connection-age, use default 1728000 (20 days)" << std::endl;
        maxIdlePersistentConnectionAge_=1728000;
    }else{
        maxIdlePersistentConnectionAge_=1728000;
    }
 }else{
    maxIdlePersistentConnectionAge_=1728000;
 }
 if (progOptions.is_set("infinite-persistent-connection-age")) {
    infinitePersistentConnectionAge_ = true;
 }else{
    infinitePersistentConnectionAge_ = false;
 }
 if (progOptions.is_set("debug-level")) {
    const char* szDebugLevel=progOptions.argument("debug-level");
    debugLevel_=atol(szDebugLevel);
 }else{
    debugLevel_=0; 
 }
#ifdef CORBA_HAVE_OTS
 CORBA::ULong nPolicies=5;
#else
 CORBA::ULong nPolicies=4;
#endif
 CORBA::PolicyList policyList;
 policyList.length(nPolicies);
 policyList[0]=systemPOA_->create_lifespan_policy(PortableServer::TRANSIENT);
 policyList[1]=systemPOA_->create_id_assignment_policy(PortableServer::SYSTEM_ID);
 policyList[2]=systemPOA_->create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
 policyList[3]=systemPOA_->create_implicit_activation_policy(PortableServer::IMPLICIT_ACTIVATION);
#ifdef CORBA_HAVE_OTS
 CORBA::Any any;
# if defined(OB_INTEGER_VERSION)
#  if (OB_INTEGER_VERSION < 4000400L)
      any <<= CosPolicyDef::ADAPTS;
      policyList[4]=orb->create_policy(CosPolicyDef::OTSPolicyType,any);
#  else
      any <<= CosTransactions::ADAPTS;
      policyList[4]=orb->create_policy(CosTransactions::OTS_POLICY_TYPE,any);
#  endif
#else
  // NOT ORbacus, use specs
  any <<= CosTransactions::ADAPTS;
  policyList[4]=orb->create_policy(CosTransactions::OTS_POLICY_TYPE,any);
# endif
#endif
 queriesPOA_ = systemPOA_->create_POA("queries", 
                              PortableServer::POAManager::_nil(), policyList);
 PortableServer::POAManager_var poaManager = queriesPOA_->the_POAManager();
 poaManager->activate();
 
 //TODO:
 // define policy list and Object Activator.
 // for rssh
 policyList.length(5);
 policyList[0]=systemPOA_->create_lifespan_policy(PortableServer::PERSISTENT);
 policyList[1]=systemPOA_->create_id_assignment_policy(PortableServer::USER_ID);
 policyList[2]=systemPOA_->create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
 policyList[3]=systemPOA_->create_servant_retention_policy(PortableServer::RETAIN);
 policyList[4]=systemPOA->create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);


 connectionsPOA_ = systemPOA_->create_POA("connections",
                              PortableServer::POAManager::_nil(),policyList);
 DBConnectionServantActivator* dbca =new DBConnectionServantActivator(this);
 connectionsPOA_->set_servant_manager(dbca);
 poaManager = connectionsPOA_->the_POAManager();
 poaManager->activate();
 if (progOptions_.is_set("cb-fname")) {
     connectionsBackupFname_=progOptions_.argument("cb-fname");
 }
 try {
   loadConnectionsBackup();
 }catch(const std::runtime_error& ex){
   GradSoft::Logger::ErrorLocker ml(getLogger().errors());
   if (getDebugLevel()>5) {
           getLogger().errors() << "[" << __FILE__ << ":" << __LINE__ << "]";
   }
   getLogger().errors() 
      << "error during reading connections storage, ignoring: " << std::endl
      << ex.what() << std::endl;
 }
 nurser_p_=new Nurser(this);
 nurser_p_->init();
 nurser_p_->start();
}

DBConnectionManagers_impl::~DBConnectionManagers_impl()
{
 if (nurser_p_) {
   nurser_p_->setQuit(true);
   GradSoft::Thread::join(*nurser_p_);
   delete nurser_p_;
 }
}


UAKGQuery2::QueryManager_ptr DBConnectionManagers_impl::create_query_manager(
		                                    const char* dbLogin,
		                                    const char* dbPassword,
	                                            const char* dbName, 
						    const char* dbDriver,
                                                    const char* dbOther)
                              throw (CORBA::SystemException, 
                                     UAKGQuery2::QueryProcessingError, 
                                     UAKGQuery2::QueryManagerNotFound)
{
    DBConnectionParams params(dbDriver,dbName,dbLogin,dbPassword,dbOther);
    UAKGQueryManagerBase* qmServant=createUAKGQueryManager_(&params);
    return createQMReference(qmServant,&params);
}


UAKGQueryManagerBase* DBConnectionManagers_impl::createUAKGQueryManager_(
		                           const DBConnectionParams* params_p)
{
 DBConnectionManagerBase* target_p = findDriver(params_p->getDriverName());
 return target_p->createUAKGQueryManager_(this,params_p);
}

UAKGQuery2::QueryManager_ptr DBConnectionManagers_impl::createQMReference(UAKGQueryManagerBase* qmServant, const DBConnectionParams* params_p)
{
    long newId;
	try {
	  newId=generateNextConnectionId();
	}catch(const ConnectionIdGenerationFailed& ){
          GradSoft::Logger::ErrorLocker ml(getLogger().errors());
          if (getDebugLevel() > 5) {
            getLogger().errors() << "[" << __FILE__ << ":" << __LINE__ << "]";
          }
	  getLogger().errors() << "Can't generate new if for connection reference, old id=" << connectionsCurrentId_ << std::endl;
		throw CORBA::OBJ_ADAPTER();
	}

	{
	 GradSoft::MutexLocker ml(connectionsStorageMutex_);
	 connectionsStorage_[newId]=ConnectionRecord(*params_p,qmServant);
         connectionsStorageLastTouch_=time(NULL);
	}
	char soid[10];
	sprintf(soid,"%d",newId);
	getLogger().infos() <<  "generate oid for " << soid << std::endl;
	PortableServer::ObjectId_var oid=PortableServer::string_to_ObjectId(soid);
	connectionsPOA_->activate_object_with_id(oid,qmServant);

	CORBA::Object_var o=connectionsPOA_->create_reference_with_id(oid,"IDL:gradsoft.kiev.ua/UAKGQuery2/QueryManager:1.0");
	UAKGQuery2::QueryManager_var retval=UAKGQuery2::QueryManager::_narrow(o);
	return retval._retn();
}

long DBConnectionManagers_impl::getConnectionRecordId(const char* soid) 
{
 char* endptr;
 long id = strtol(soid,&endptr,10);
 if (endptr==NULL) {
   std::ostringstream message;
   message << "Invalid connection id - strtol(soid) failed, soid=" << soid ;
   GradSoft::Logger::ErrorLocker l(getLogger().errors());
   if (getDebugLevel() > 5) { 
            getLogger().errors() << "[" << __FILE__ << ":" << __LINE__ << "]";
   }
   getLogger().errors() << message.str() << std::endl;
   throw std::invalid_argument(message.str());
 }
 return id;
}

DBConnectionManagers_impl::ConnectionRecord*      
    DBConnectionManagers_impl::findConnectionRecordPtr(const char* soid) 
{
 return findConnectionRecordPtr(getConnectionRecordId(soid));
}


DBConnectionManagers_impl::ConnectionRecord*   
	 DBConnectionManagers_impl::findConnectionRecordPtr(long id) 
{
 GradSoft::MutexLocker ml(connectionsStorageMutex_);
 ConnectionsStorageIteratorType it=connectionsStorage_.find(id);
 if (it==connectionsStorage_.end()) {
	throw ConnectionRecordNotFound();
 }
 return &((*it).second);
}

void DBConnectionManagers_impl::printConnectionsStorage(std::ostream& out)
{ 
 for(ConnectionsStorageIteratorType it=connectionsStorage_.begin();
		 it!=connectionsStorage_.end(); ++it)
 {
   out << (*it).first << "," << (*it).second.second << std::endl;
 }
}


  
void  DBConnectionManagers_impl::registerDBDriver(const char* drv_name,
                           DBConnectionManagerBase*  connectionManager,
                           bool ownity)
{
 DBDriverParams dbParams(drv_name,false);
 driversStorage_[dbParams]=DBDriverRecord(connectionManager,ownity);
 if (ownity) {
   connectionManager->init(progOptions_,logger_,connectionsPOA_.in());
 }else{
   // otherwise they are initialized by call, when ownity set to true.
   // so, do nothing.
 }
}


DBConnectionManagerBase* DBConnectionManagers_impl::findDriver(
		                                    const char* drv_name) 
{
 DBDriverParams dbParams(drv_name,false);
 DriversStorageType::iterator it = driversStorage_.find(dbParams);
 if (it==driversStorage_.end()) throw UAKGQuery2::QueryManagerNotFound();
 return ((*it).second).first;
}
  

void DBConnectionManagers_impl::shutdown() throw (CORBA::SystemException)
{
  orb_->shutdown(false);
  //think - may be shutdown all connections also here ?
  shutdowned_ = 1;
}

long DBConnectionManagers_impl::generateNextConnectionId()
{
	GradSoft::MutexLocker ml(connectionsStorageMutex_);
	int nTries=0;
	const int maxNTries=10000;
	long retval=++connectionsCurrentId_;
	ConnectionsStorageIteratorType it=connectionsStorage_.find(retval);
	while(it!=connectionsStorage_.end() && nTries < maxNTries) {
       retval=++connectionsCurrentId_;
	   it=connectionsStorage_.find(retval);
	   ++nTries;
	}
	if (nTries==maxNTries) {
		// near impossible.
		throw ConnectionIdGenerationFailed();
	}
	return retval;
}

void DBConnectionManagers_impl::setCurrentConnectionId(long newId)
{
  GradSoft::MutexLocker ml(connectionsStorageMutex_);
  connectionsCurrentId_=newId;
}

void DBConnectionManagers_impl::loadConnectionsBackup()
{
//TODO: hold connections POA manager
  std::ifstream inf;
  inf.open(connectionsBackupFname_.c_str(),std::ios::in);
  if (!inf) {
    getLogger().warnings() << "file " << connectionsBackupFname_.c_str() 
	                   << " does not exists, first-time call ?"
			   << std::endl;
    return;
  }
  ConnectionRecord cr;
  int nRecords=0;
  {
   time_t now = time(0);
  GradSoft::MutexLocker ml(connectionsStorageMutex_);
   while(!inf.eof()) {
     connectionsCurrentId_=readOneConnectionRecord(inf,cr,nRecords);
     if (connectionsCurrentId_==-1) {
	     continue;
     } 
     if (!infinitePersistentConnectionAge_) {
       if (now - cr.first.getLastTouch() > maxIdlePersistentConnectionAge_) {
             getLogger().infos() << 
		     "age of connection with id " << connectionsCurrentId_ <<
		     " is more than max-idle-persistent-connection-age, forget"
		      << std::endl; 
	     continue;
       }
     }
     connectionsStorage_[connectionsCurrentId_]=cr;
     ++nRecords;
   }
  }
  time(&connectionsStorageLastTouch_);
//TODO: unhold collections POA manager
}

void DBConnectionManagers_impl::backupConnections()
{
  std::string backupBackupFname=connectionsBackupFname_+".bak";
  int rc=rename(connectionsBackupFname_.c_str(),backupBackupFname.c_str());
  if (rc==-1) {
      std::string action="rename "+connectionsBackupFname_+
	                        " to "+backupBackupFname;
      if (errno==ENOENT) {
        getLogger().warnings() << "action " << action << " returns ENOENT" << std::endl;
        getLogger().warnings() << "first-time call ?" << std::endl;
      }else{
        std::string message;
#ifdef HAVE_STRERROR_R
        char szmessage[255];
        strerror_r(errno,szmessage,255);
        message=std::string(szmessage);
#else
        char* szmessage=strerror(errno);
        message=std::string(szmessage);
#endif
        throw FileIOException(action,message,errno);
     }
  }
  std::ofstream ofs;
  try {
    ofs.open(connectionsBackupFname_.c_str());
  }catch(const std::exception& ex) {
    std::string action="open file "+connectionsBackupFname_+" for writing";
    throw FileIOException(action,ex.what(),errno);
  }
  if (!ofs) {
    std::string action="open file "+connectionsBackupFname_+" for writing";
    throw FileIOException(action,"can't open file",errno);
  }
  ConnectionsStorageType localStorage;
  {
   GradSoft::MutexLocker ml(connectionsStorageMutex_);
#ifdef _MSC_VER
   ConnectionsStorageType::iterator src=connectionsStorage_.begin();
   ConnectionsStorageType::iterator dst=localStorage.begin();
   while(src!=connectionsStorage_.end()) {
     dst=localStorage.insert(dst,*src++);
   }

#else
   localStorage.insert(connectionsStorage_.begin(),connectionsStorage_.end());
#endif
  }
  ConnectionsStorageIteratorType it;
  int nRecord=0;
  for(it=localStorage.begin(); it!=localStorage.end(); ++it,++nRecord)
  {
     writeOneConnectionRecord(ofs,(*it).first,(*it).second,nRecord);
  }
  ofs.close();
  {
   GradSoft::Logger::InfoLocker l(getLogger().infos());
   getLogger().infos() << "backup of connection info is done. " << nRecord
	               << " connections" << std::endl;
  }
}

/*
 * How connections are stored into file ?
 *   One connection record contains next information:
 *    1. OID
 *    2. std::string driverName_;
 *    3. std::string dbName_;
 *    4. std::string login_;
 *    5. std::string password_;
 *    6. std::string other_;
 *
 *  Each connection record occypies one line and contains coded fields, 
 *  separated by symbols :1:, :2:, :3:, :4:, :5:, :6:.
 *   if symbol sequence contains ::, it changed to : 
 *     
 */

void  DBConnectionManagers_impl::writeOneConnectionRecord(std::ostream& out,
             long id,
             const ConnectionRecord&  cr,
	     int nRecord)
{
 out << ":1:"; 
 std::ostringstream ostr; 
 ostr << id;
 out << UAKGQueryUtils::code(ostr.str(),':',cbKey_);
 out << ":2:"; 
 out << UAKGQueryUtils::code(cr.first.getDriverName(),':',cbKey_);
 out << ":3:"; 
 out << UAKGQueryUtils::code(cr.first.getDBName(),':',cbKey_);
 out << ":4:"; 
 out << UAKGQueryUtils::code(cr.first.getLogin(),':',cbKey_);
 out << ":5:"; 
 out << UAKGQueryUtils::code(cr.first.getPassword(),':',cbKey_);
 out << ":6:"; 
 out << UAKGQueryUtils::code(cr.first.getOther(),':',cbKey_);
 out << ":7:"; 
 if (cr.second == NULL) {
    std::ostringstream ostr;
    ostr << cr.first.getLastTouch();
    out << UAKGQueryUtils::code(ostr.str().c_str(),':',cbKey_);
 }else{
    std::ostringstream ostr;
    ostr << cr.second->getLastTouch();
    out << UAKGQueryUtils::code(ostr.str().c_str(),':',cbKey_);
 }
 out << std::endl;
}

/**
 * read One ConnectionRecord
 * if in have correct line with connection record
 *           - fill cr and return id of record.
 * if in have incorrect line:
 *           - throw InvalidRecordException
 * if in have empty:
 *           - return -1;
 **/
long  DBConnectionManagers_impl::readOneConnectionRecord(std::istream& in,
                          DBConnectionManagers_impl::ConnectionRecord&  cr,
			  int nRecord)
{
 char buff[1000];
 in.getline(buff,999);
 std::string line(buff);
 if (line.length()==0) {
    return -1;
 }
 if (line.length() < 4) {
	 throw InvalidRecordFormat(0,nRecord,
		                  "1-st field delimiter is not found" );
 }
 if (line[0]!=':' || line[1]!='1' || line[2]!=':') {
	 throw InvalidRecordFormat(0,nRecord,
		                  "1-st field delimiter is not found" );
 }
 int pos0=3;
 int pos=line.find(":2:",pos0);
 if (pos==std::string::npos) {
	 throw InvalidRecordFormat(pos0,nRecord,
			             "2-nd field delimiter is not found" );
 }
 std::string field=UAKGQueryUtils::uncode(line.substr(pos0,pos-pos0),':',cbKey_);
 char* endptr=NULL;
 long id=strtol(field.c_str(),&endptr,10);
 if (endptr!=NULL && *endptr!='\0') {
	 throw InvalidRecordFormat(pos0,nRecord,
		                  "1-st field is invalid" );
 }
 pos0=pos+3;
 pos=line.find(":3:",pos0);
 if (pos==-1) {
	 throw InvalidRecordFormat(pos0,nRecord,
			             "3-rd field delimiter is not found" );
 }
 field=UAKGQueryUtils::uncode(line.substr(pos0,pos-pos0),':',cbKey_);
 cr.first.setDriverName(field);
 pos0=pos+3;
 pos=line.find(":4:",pos0);
 if (pos==std::string::npos) {
	 throw InvalidRecordFormat(pos0,nRecord,
			             "4-th field delimiter is not found" );
 }
 field=UAKGQueryUtils::uncode(line.substr(pos0,pos-pos0),':',cbKey_);
 cr.first.setDBName(field);
 pos0=pos+3;
 pos=line.find(":5:",pos0);
 if (pos==std::string::npos) {
	 throw InvalidRecordFormat(pos0,nRecord,
			             "5-th field delimiter is not found" );
 }
 field=UAKGQueryUtils::uncode(line.substr(pos0,pos-pos0),':',cbKey_);
 cr.first.setLogin(field);
 pos0=pos+3;
 pos=line.find(":6:",pos0);
 if (pos==std::string::npos) {
	 throw InvalidRecordFormat(pos0,nRecord,
			             "6-th field delimiter is not found" );
 }
 field=UAKGQueryUtils::uncode(line.substr(pos0,pos-pos0),':',cbKey_);
 cr.first.setPassword(field);
 pos0=pos+3;
 pos=line.find(":7:",pos0);
 if (pos==std::string::npos) {
	 // this can be, when we have cbUAKGQuery from previoud version
     field=UAKGQueryUtils::uncode(line.substr(pos0,line.length()-pos0),':',cbKey_);
     cr.first.setOther(field);
     cr.first.setLastTouch(time(0));
 }else{
     field=UAKGQueryUtils::uncode(line.substr(pos0,pos-pos0),':',cbKey_);
     cr.first.setOther(field);
     pos0=pos+3;
     field=UAKGQueryUtils::uncode(line.substr(pos0,line.length()-pos0),':',cbKey_);
     long l=atol(field.c_str());
     if (l==0) {
	 throw InvalidRecordFormat(pos0,nRecord,
			             "7-th field must be coded number" );
     }
     cr.first.setLastTouch(l);
 }
 cr.second=NULL;
 return id;
}


void DBConnectionManagers_impl::evictConnections()
{
  ConnectionsStorageType localStorage;
  {
   GradSoft::MutexLocker ml(connectionsStorageMutex_);
#ifdef _MSC_VER
   ConnectionsStorageType::iterator src=connectionsStorage_.begin();
   ConnectionsStorageType::iterator dst=localStorage.begin();
   while(src!=connectionsStorage_.end()) {
     dst=localStorage.insert(dst,*src++);
   }
#else
   localStorage.insert(connectionsStorage_.begin(),connectionsStorage_.end());
#endif
  }
  ConnectionsStorageIteratorType it=localStorage.begin();

  for(;it!=localStorage.end();++it) {
    long curId=(*it).first;
    time_t now=time(NULL);
    ConnectionRecord& curCr=(*it).second;
    if (curCr.second!=NULL) {
       if (maxIdleConnectionAge_!=0 && now-curCr.second->getLastTouch() > maxIdleConnectionAge_ ) {
	  getLogger().infos() << "evict connection " << curId << std::endl;
          std::ostringstream ostr;
	  ostr << curId;
	  PortableServer::ObjectId_var oid;
	  try {
             oid=PortableServer::string_to_ObjectId(ostr.str().c_str());
	  }catch(const CORBA::SystemException& ex) {
	     GradSoft::Logger::ErrorLocker l(getLogger().errors());
             if (getDebugLevel()>5) {
               getLogger().errors() << "["<<__FILE__<<":"<<__LINE__<<"]";
             }
             getLogger().errors() << "error during evict of connection " 
		                  << curId << ":" << ex << std::endl;
	     continue;
	  }
	  try {
             connectionsPOA_->deactivate_object(oid);
	  }catch(const CORBA::SystemException& ex){
	     GradSoft::Logger::ErrorLocker l(getLogger().errors());
             if (getDebugLevel()>5) {
               getLogger().errors() << "["<<__FILE__<<":"<<__LINE__<<"]";
             }
             getLogger().errors() << "error during deactivation of connection " 
		                  << curId << ":" << ex << std::endl;
	  }catch(const std::exception& ex){
	     GradSoft::Logger::ErrorLocker l(getLogger().errors());
             if (getDebugLevel()>5) {
               getLogger().errors() << "["<<__FILE__<<":"<<__LINE__<<"]";
             }
             getLogger().errors() << "error during evict " << ex.what() << std::endl;
	  }
       }
    }
  }
}
