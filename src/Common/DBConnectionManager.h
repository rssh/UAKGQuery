#ifndef __Common_DBConnectionManager_h
#define __Common_DBConnectionManager_h

/*
 * part of GradSoft UAKGQuery
 * (C) GradSoft 1998-2003
 * http://www.gradsoft.com.ua
 * $Id: DBConnectionManager.h,v 1.39 2007-12-26 13:44:56 rssh Exp $
 */

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __GradSoft_Logger_h
#include <GradSoft/Logger.h>
#endif

#ifndef __GradSoft_ProgOptions_h
#include <GradSoft/ProgOptions.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __Common_UAKGQueryManagerBase_h
#include <Common/UAKGQueryManagerBase.h>
#endif

#ifndef __Common_Nurser_h
#include <Common/Nurser.h>
#endif

#include <assert.h>
#include <time.h>

#include <algorithm>
#include <functional>
#include <map>


struct DBDriverParams
{
 char* driverName_;
 bool allocated_;
 DBDriverParams(const char* drv_name, bool allocate);
 DBDriverParams(const DBDriverParams&);
 DBDriverParams& operator=(const DBDriverParams&);
 ~DBDriverParams();
};


class DBConnectionParams
{
private:

	std::string driverName_;
	std::string dbName_;
	std::string login_;
	std::string password_;
	std::string other_;
	// time of last active operation.
	//    pushed to parameters during connections etherealizing
	//    restored during load backup.
	time_t      lastTouch_;

public:

	DBConnectionParams()
		:driverName_(),dbName_(),login_(),password_(),other_() {}

	DBConnectionParams(const char* drvName,
		               const char* dbName,
					   const char* login,
					   const char* password,
					   const char* other)
	  :driverName_(drvName),
	   dbName_(dbName),
	   login_(login),
	   password_(password),
	   other_(other)
	{}

	DBConnectionParams(const DBConnectionParams& x)
		:driverName_(x.driverName_),
		 dbName_(x.dbName_),
		 login_(x.login_),
		 password_(x.password_),
		 other_(x.other_)
	{}

	DBConnectionParams& operator=(const DBConnectionParams& x)
	{
		if (&x!=this) {
			driverName_=x.driverName_;
			dbName_=x.dbName_;
			login_=x.login_;
			password_=x.password_;
			other_=x.other_;
		}
		return *this;
	}
	      

    const char*  getDriverName() const
	{ return driverName_.c_str(); }

    void  setDriverName(const std::string& driverName)
        { driverName_ = driverName; }

	const char*  getDBName() const
	{ return dbName_.c_str(); }

	void  setDBName(const std::string& dbName)
	{ dbName_ = dbName; }

	const char*  getLogin() const
	{ return login_.c_str(); }

	void  setLogin(const std::string& login)
	{ login_=login; }

	const char*  getPassword() const
	{ return password_.c_str(); }

	void  setPassword(const std::string& password)
	{ password_ = password; }

	const char*  getOther() const
	{ return other_.c_str(); }

	void  setOther(const std::string& other)
	{ other_=other; }

	time_t getLastTouch() const
	{ return lastTouch_; }

	void  setLastTouch(const time_t lastTouch)
	{
	  lastTouch_ = lastTouch;
	}

     void print(std::ostream& out) const
     { out << "(login=" << login_ << ", password="<< password_
	   <<  ", dbname=" << dbName_ << ", dbdriver=" << driverName_
           <<  ", other=" << other_ << " lastTouch=" << lastTouch_ << ")"; 
     }


};

inline
std::ostream& operator<<(std::ostream& out, const DBConnectionParams& params)
{
 params.print(out);
 return out;
}

struct DBDriverParamsLess: public std::binary_function<DBDriverParams, DBDriverParams, bool>
{
  bool operator()(const DBDriverParams& x, const DBDriverParams& y) const;
};


class DBConnectionManagers_impl;


class DBConnectionManagerBase: 
    virtual public POA_UAKGQuery2::DBConnectionManager,
    virtual public PortableServer::RefCountServantBase
{
private:

  GradSoft::ProgOptions*  progOptions_p_;
  GradSoft::Logger*       logger_p_;
  PortableServer::POA_var poa_;

public:


  virtual void init(GradSoft::ProgOptions& progOptions, 
                    GradSoft::Logger& logger,
                    PortableServer::POA_ptr myPOA);

  virtual ~DBConnectionManagerBase() {}


 
  virtual UAKGQuery2::QueryManager_ptr create_query_manager(
		     const char* dbLogin,
		     const char* dbPassword,
	             const char* dbName, const char* dbDriver,
                     const char* dbOther)
                              throw (CORBA::SystemException, 
                                     UAKGQuery2::QueryProcessingError, 
                                     UAKGQuery2::QueryManagerNotFound)
  {
    assert(" creation of internal ConnectionManager is impossible"==NULL);
    return UAKGQuery2::QueryManager::_nil();
  }


  PortableServer::POA_ptr _default_POA()
     { return PortableServer::POA::_duplicate(poa_); }


  void shutdown()
           throw(CORBA::SystemException)
  {
    assert(" shutdown of internal ConnectionManager is impossible"==NULL);
  }


  //
  // real internal creation of query manager
  //
  virtual UAKGQueryManagerBase* 
           createUAKGQueryManager_(DBConnectionManagers_impl* cm,
		                           const DBConnectionParams* cnp) = 0;
  

  //
  // Access to logger
  //
  GradSoft::Logger&   getLogger()  { return *logger_p_; }

  GradSoft::DebugLogStream&    debugs()    { return logger_p_->debugs(); }
  GradSoft::InfoLogStream&     infos()     { return logger_p_->infos(); }
  GradSoft::WarningLogStream&  warnings()  { return logger_p_->warnings(); }
  GradSoft::ErrorLogStream&    errors()    { return logger_p_->errors(); }
  GradSoft::FatalLogStream&    fatals()    { return logger_p_->fatals(); }


protected:

  DBConnectionManagerBase() {}

private:

  DBConnectionManagerBase(const DBConnectionManagerBase&); 
  DBConnectionManagerBase& operator=(const DBConnectionManagerBase&); 

};


class DBConnectionManagers_impl:
    virtual public POA_UAKGQuery2::DBConnectionManager,
    virtual public PortableServer::RefCountServantBase
{
public:

  typedef std::pair<DBConnectionParams,UAKGQueryManagerBase*>  ConnectionRecord;

private:

  typedef std::map<long,ConnectionRecord> ConnectionsStorageType;

  typedef ConnectionsStorageType::iterator ConnectionsStorageIteratorType;

  typedef std::pair<DBConnectionManagerBase*,bool> DBDriverRecord;

  typedef std::map<DBDriverParams, 
              DBDriverRecord, 
              DBDriverParamsLess>   DriversStorageType; 

  typedef DriversStorageType::iterator DriversStorageIteratorType;


  /**
   * drivers, to which we delegate actual creation of connections.
   **/
  DriversStorageType      driversStorage_;

  /**
   * we keep all open connections there.
   **/
  ConnectionsStorageType  connectionsStorage_;

  GradSoft::Mutex         connectionsStorageMutex_;
  /**
   * used for generation of connections object key.
   *@see  generateNextConnectionId.
   **/
  long                    connectionsCurrentId_;

  /**
   * marked, when we touch connectionsStorage_.
   *  used for evicting and file updating.
   **/
  time_t                  connectionsStorageLastTouch_;

  /**
   * file name for backup file, where connections are stored.
   **/
  std::string  connectionsBackupFname_;

  PortableServer::POA_var systemPOA_;
  PortableServer::POA_var queriesPOA_;
  PortableServer::POA_var connectionsPOA_;
  PortableServer::POA_var myPOA_;
  CORBA::ORB_var orb_;

  CORBA::Boolean shutdowned_;

  GradSoft::ProgOptions& progOptions_;
  GradSoft::Logger&      logger_;
  bool         logSQL_;
  int          maxIdleConnectionAge_;
  int          maxIdlePersistentConnectionAge_;
  bool         infinitePersistentConnectionAge_;

  /**
   * Key is used for coding of connections in backup file.
   **/
  std::string  cbKey_;

  Nurser*  nurser_p_;

  int      debugLevel_;

public:

	struct ConnectionRecordNotFound: public std::runtime_error 
	{
          ConnectionRecordNotFound()
		  :std::runtime_error("connection record not found")
		  {}
	};
	struct ConnectionIdGenerationFailed: public std::runtime_error 
	{
          ConnectionIdGenerationFailed()
		  :std::runtime_error("generation id failed")
		  {}
	};
	struct FileIOException: public std::runtime_error 
	{
          FileIOException(const std::string& aAction, 
			  const std::string& aErrmsg,
			  int   aErrno)
		  :std::runtime_error(std::string("file io, action - ")
				      +aAction + ":" + aErrmsg),
                   action(aAction),
		   errmsg(aErrmsg),
		   errorCode(aErrno)
		   {}
	  ~FileIOException() throw()  {}

          std::string action;
          std::string errmsg;
	  int  errorCode;
	};
	struct InvalidRecordFormat: public std::runtime_error 
	{
          InvalidRecordFormat(int pos,int line,const std::string& msg)
		  :std::runtime_error(
				  std::string("Invalid record format at line ")+
                                  UAKGQueryUtils::longToString(line)+" "+
				  UAKGQueryUtils::longToString(pos)+":"+msg)
		  {}
	};


	       
public:

  DBConnectionManagers_impl(CORBA::ORB_ptr orb, 
                            PortableServer::POA_ptr systemPOA,
                            PortableServer::POA_ptr userPOA,
                            GradSoft::ProgOptions& progOptions,
                            GradSoft::Logger&      logger); 


  ~DBConnectionManagers_impl();
  
  void  registerDBDriver(const char* drv_name,
                         DBConnectionManagerBase*  connectionManager,
                         bool ownity);

  void  unregisterDBDriver(const char* drv_name);


  DBConnectionManagerBase* findDriver(const char* drv_name);
  
  /**
   * find ConnectionRecord by object key.
   *@exception
   *   ConnectionRecordNotFound
   *   std::invalid_argument if soid is invalid.
   **/
  ConnectionRecord*      findConnectionRecordPtr(const char* soid); 


  /**
   * find ConnectionRecord by object id.
   *@exception
   *   ConnectionRecordNotFound
   **/
  ConnectionRecord*      findConnectionRecordPtr(long id); 


  /**
   * retrieve connection record if from object key.
   *@exception
   *   std::invalid_argument if soid is invalid.
   **/
  long                   getConnectionRecordId(const char* soid);
  

  //
  // debug.
  //

  /*
   * print connections storage.
   * usage for debug.
   */
  void printConnectionsStorage(std::ostream& out);

  //
  // IDL stuff
  //
  UAKGQuery2::QueryManager_ptr create_query_manager(const char* dbLogin,
		                                    const char* dbPassword,
	                                            const char* dbName, 
						    const char* dbDriver,
                                                    const char* dbOther)
                                      throw (CORBA::SystemException, 
                                             UAKGQuery2::QueryProcessingError, 
                                             UAKGQuery2::QueryManagerNotFound);


  UAKGQueryManagerBase* createUAKGQueryManager_(const DBConnectionParams* params_p);

  void shutdown() throw (CORBA::SystemException);

  CORBA::Boolean isShutdowned() const { return shutdowned_; }

  GradSoft::Logger& getLogger() { return logger_; }
  const GradSoft::Logger& getLogger() const { return logger_; }
  bool    logSQL()  const { return logSQL_; }


  bool   getDebugLevel() const
   { return debugLevel_; }

//
//
  PortableServer::POA_ptr   _default_POA() 
               { return PortableServer::POA::_duplicate(myPOA_.in()); }

  PortableServer::POA_ptr   getSystemPOA_()  { return systemPOA_.in(); }
  PortableServer::POA_ptr   getSystemPOA()  
                { return PortableServer::POA::_duplicate(systemPOA_.in()); }

  PortableServer::POA_ptr   getQueriesPOA_()  { return queriesPOA_.in(); }
  PortableServer::POA_ptr   getQueriesPOA()  
              { return PortableServer::POA::_duplicate(queriesPOA_.in()); }

  PortableServer::POA_ptr   getConnectionsPOA_()  
                               { return connectionsPOA_.in(); }
  PortableServer::POA_ptr   getConnectionsPOA()  
          { return PortableServer::POA::_duplicate(connectionsPOA_.in()); }

  CORBA::ORB_ptr getORB_()  { return orb_.in(); }

private:
    //
	friend class DBConnectionServantActivator;
	
	long generateNextConnectionId();
	void setCurrentConnectionId(long newId);

	/**
     * create reference for servant and register one in connectionsStorage_;
     **/
  UAKGQuery2::QueryManager_ptr createQMReference(UAKGQueryManagerBase* qmServant,const DBConnectionParams* params_p);


public:

  void  removeQMReference(const char* sid)
  {
      removeQMReference(getConnectionRecordId(sid));
  }

  void  removeQMReference(long id)
  {
	  GradSoft::MutexLocker ml(connectionsStorageMutex_);
	  connectionsStorage_.erase(id);
	  connectionsStorageLastTouch_=time(NULL);
  }

private:

  void loadConnectionsBackup();

public:

  time_t getConnectionsLastTouch() 
  { 
    GradSoft::MutexLocker ml(connectionsStorageMutex_);
    return connectionsStorageLastTouch_; 
  }

  void backupConnections();

  void evictConnections();

private:
  long  readOneConnectionRecord(std::istream& in,
                                ConnectionRecord&  cr,
			        int nRecord);

  void  writeOneConnectionRecord(std::ostream& out,
             long id,
             const ConnectionRecord&  cr,
	     int nRecord);


public:


private:

  DBConnectionManagers_impl(const DBConnectionManagers_impl&);
  DBConnectionManagers_impl& operator=(const DBConnectionManagers_impl&);

};
#endif
