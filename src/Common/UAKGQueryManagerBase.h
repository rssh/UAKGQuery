#ifndef __Common_UAKGQueryManagerBase_h
#define __Common_UAKGQueryManagerBase_h

/*
 * part of GradSoft UAKGQuery
 * (C) GradSoft 1998, 1999, 2000, 2001
 * $Id: UAKGQueryManagerBase.h,v 1.53 2008-02-13 17:51:50 rssh Exp $
 */

#ifndef __Common_StringFlagSet_h
// first, to include __USE_UNIX98 for Linux
#include <Common/StringFlagSet.h>
#endif

#ifndef __GradSoft_Logger_h
#include <GradSoft/Logger.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __Common_UAKGQueryUtils_h
#include <Common/UAKGQueryUtils.h>
#endif

#ifndef __RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#ifdef CORBA_HAVE_OTS
#include COSTRANSACTIONS_H
#endif

#ifndef __Common_UAKGLobs_h
#include <Common/UAKGLobs.h>
#endif

#include <time.h>

class DBConnectionManagers_impl;
class DBConnectionParams;

class UAKGQueryBase;

class UAKGQueryManagerBase: 
		   virtual public POA_UAKGQuery2::QueryManager,
		   virtual public POA_UAKGQuery2::QueryEvaluator,
                   virtual public PortableServer::RefCountServantBase
{
protected:

  friend class UAKGQueryBase;

  char* username_;
  char* password_;
  char* dbLink_;
  char* szFlags_;
  StringFlagSet flags_;
  StringFlagSet properties_;

  CORBA::ULong nQueries_;
  mutable GradSoft::Mutex nQueriesMutex_;

  DBConnectionManagers_impl* dbConnectionManagers_p_;
  const char* dbName_p_;

  time_t  lastTouch_;
  mutable GradSoft::Mutex touchMutex_;

  PortableServer::POA_var blobWritingPOA_;
  PortableServer::POA_var clobWritingPOA_;
  PortableServer::POA_var wclobWritingPOA_;
  PortableServer::POA_var blobReadingPOA_;
  PortableServer::POA_var clobReadingPOA_;
  PortableServer::POA_var wclobReadingPOA_;

  // default servants for appropriative poa-s
  UAKGLobBase* blobWritingServantPtr_;
  UAKGLobBase* clobWritingServantPtr_;
  UAKGLobBase* wclobWritingServantPtr_;
  UAKGLobBase* blobReadingServantPtr_;
  UAKGLobBase* clobReadingServantPtr_;
  UAKGLobBase* wclobReadingServantPtr_;

  /**
   * is set to true, when we must destroy persistent reference to
   * query manager, otherwise - false.
   *
   * it is set in destroy method and value is checked in
   * DBConnectionsServantActivator::etherealize, which is called 
   * asynchronically (i. e. after destroy method).
   **/
  bool isDestroyed_;

  PortableServer::POA_ptr getLobPOA_(const char* namePrefix
          , PortableServer::ServantBase* serv);

private:

  virtual UAKGLobBase* createBlobReadingServant() = 0;
  virtual UAKGLobBase* createClobReadingServant() = 0;
  virtual UAKGLobBase* createWclobReadingServant() = 0;
  virtual UAKGLobBase* createBlobWritingServant() = 0;
  virtual UAKGLobBase* createClobWritingServant() = 0;
  virtual UAKGLobBase* createWclobWritingServant() = 0;

public:

  PortableServer::POA_ptr getBlobWritingPOA();
  PortableServer::POA_ptr getClobWritingPOA();
  PortableServer::POA_ptr getWclobWritingPOA();
  PortableServer::POA_ptr getBlobReadingPOA();
  PortableServer::POA_ptr getClobReadingPOA();
  PortableServer::POA_ptr getWclobReadingPOA();

  // the same, but without duplicate
  PortableServer::POA_ptr getBlobWritingPOA_();
  PortableServer::POA_ptr getClobWritingPOA_();
  PortableServer::POA_ptr getWclobWritingPOA_();
  PortableServer::POA_ptr getWclobReadingPOA_();
  PortableServer::POA_ptr getBlobReadingPOA_();
  PortableServer::POA_ptr getClobReadingPOA_();

private:

  /**
   * create POA with default servant.
   **/
  PortableServer::POA_ptr createLobPOA_(const char* name_prefix, 
                                        UAKGLobBase* lobServant );

public:

  char*  getDBLink_p() const { return checkStr(dbLink_); }
  char*  getUsername_p() const { return checkStr(username_); }
  char*  getPassword_p() const { return checkStr(password_); }
  const char*  getDBName_p() const { return dbName_p_; }

  char*  get_username() throw (CORBA::SystemException,
		               UAKGQuery2::QueryProcessingError)
   { return CORBA::string_dup(getUsername_p()); } 
  
  char*  get_dblink() throw (CORBA::SystemException,
		             UAKGQuery2::QueryProcessingError)
   { return CORBA::string_dup(getDBLink_p()); } 

  char* get_property(const char* property_name) throw (CORBA::SystemException,
			                               UAKGQuery2::PropertyNotDefined);

  CORBA::ULong  number_of_queries() 
     { GradSoft::MutexLocker ml(nQueriesMutex_); return nQueries_; }

  void          incNQueries()  
     { GradSoft::MutexLocker ml(nQueriesMutex_); ++nQueries_; }

  void          decNQueries()  
     { GradSoft::MutexLocker ml(nQueriesMutex_); --nQueries_; }

  //
  // Utils:
  //
  GradSoft::Logger&   getLogger() const;

  GradSoft::DebugLogStream&    debugs() const  { return getLogger().debugs(); } 
  GradSoft::InfoLogStream&     infos()  const   { return getLogger().infos(); } 
  GradSoft::WarningLogStream&  warnings() const  { return getLogger().warnings(); } 
  GradSoft::ErrorLogStream&    errors() const   { return getLogger().errors(); } 
  GradSoft::FatalLogStream&    fatals() const   { return getLogger().fatals(); } 

  bool      logSQL() const  ;

  bool      isDestroyed() const 
    { return isDestroyed_; } 

  bool      getDebugLevel() const;

protected:

  char* checkStr(char* x) const;

public:

  UAKGQueryManagerBase();

  virtual ~UAKGQueryManagerBase();
  
  virtual void destroy() throw (CORBA::SystemException);

  virtual void  init( 
                    DBConnectionManagers_impl* connectionManagers_p,
		    const DBConnectionParams* params_p);

  void touch()
  { 
   GradSoft::MutexLocker ml(touchMutex_);
   time(&lastTouch_); 
  }

  time_t getLastTouch() const
  { return lastTouch_; }

  UAKGQuery2::Query_ptr create(const char* query_text, const char* flags)
      throw (CORBA::SystemException,
	     UAKGQuery2::QueryFlagInvalid)
   { return create_query(query_text,flags); }

  UAKGQuery2::Query_ptr create_query(const char* query_text, const char* flags)
	  throw (CORBA::SystemException,
		 UAKGQuery2::QueryFlagInvalid);

  virtual UAKGQueryBase* createUAKGQuery(const char* query_text, 
		                              const char* flags)
	  throw (CORBA::SystemException,
		 UAKGQuery2::QueryFlagInvalid) = 0;

  UAKGQuery2::RecordSet* evaluate( const char* query_text, 
				   const char* query_flags,
                                   UAKGQuery2::RecordSet* rs )
	  throw (CORBA::SystemException,
		 UAKGQuery2::QueryFlagInvalid,
		 UAKGQuery2::QueryProcessingError);

  UAKGQuery2::RecordSet* evaluate_e( const char* query_text, 
				     const char* query_flags)
	  throw (CORBA::SystemException,
		 UAKGQuery2::QueryFlagInvalid,
		 UAKGQuery2::QueryProcessingError);

#if defined(MICO) && MICO_BIN_VERSION < 0x02030b
  UAKGQuery2::RecordSet* evaluate_inout( const char* query_text, 
					                 const char* query_flags,
                             		 UAKGQuery2::RecordSet* rs)
	  throw (CORBA::SystemException,
		 UAKGQuery2::QueryFlagInvalid,
		 UAKGQuery2::QueryProcessingError);
#else
UAKGQuery2::RecordSet* evaluate_inout( const char* query_text, 
					                 const char* query_flags,
                             		 UAKGQuery2::RecordSet*& rs)
	throw (CORBA::SystemException,
	       UAKGQuery2::QueryFlagInvalid,
	       UAKGQuery2::QueryProcessingError);
#endif

  CORBA::Long get_next_sequence_value(const char* sequence_name) 
	  throw (CORBA::SystemException,
		 UAKGQuery2::QueryFlagInvalid,
		 UAKGQuery2::QueryProcessingError) 
  {
    touch();
    return get_next_sequence_value_internal(sequence_name);
  }

  virtual CORBA::Long get_next_sequence_value_internal(const char* sequence_name) 
	  throw (CORBA::SystemException,
		 UAKGQuery2::QueryFlagInvalid,
		 UAKGQuery2::QueryProcessingError) = 0;

//
// internal 
//

    CORBA::ORB_ptr  getORB_(); 

    PortableServer::POA_ptr getPOA_();
    PortableServer::POA_ptr _default_POA();

    PortableServer::POA_ptr getQueriesPOA_();


#ifdef CORBA_HAVE_OTS
protected:
   CosTransactions::Current_var transCurrent_;
public:


   CosTransactions::Current_ptr transCurrent()
    {
       return CosTransactions::Current::_duplicate(transCurrent_);
    } 
#endif

   const char* get_sz_flag(const char* flagName) const;

   int  get_int_flag(const char* flagName) const;

private:

  UAKGQueryManagerBase(const UAKGQueryManagerBase&);
  UAKGQueryManagerBase& operator=(const UAKGQueryManagerBase&);

//protected:
  //(gcc bug)
public:

  /**
  *  return sql adapted for custom driver
  *   memory for return value is allocated by CORBA::string_dup
  **/
  virtual char*   normalize_sql(const char* sql)=0;


};

#endif
