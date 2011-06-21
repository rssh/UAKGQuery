#ifndef __Pool_PoolQueryManager_h
#define __Pool_PoolQueryManager_h

/**
 * part of Grad-Soft UAKGQuery
 * (C) Grad-Soft Ltd. Kiev. Ukraine
 * http://www.gradsoft.kiev.ua
 * $Id: PoolQueryManager.h,v 1.5 2004-02-10 12:10:49 orchids Exp $
 **/

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __Common_UAKGQueryManagerBase_h
#include <Common/UAKGQueryManagerBase.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif

#ifndef __RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#include <vector>


/**
 * Query Manager for 'Pool'.
 *   It's contains pool of query managers,
 *       original name of database driver is the value fo DBDriver in options
 *       size of Pool is variable PoolSize in options.
 **/
class PoolQueryManager: public UAKGQueryManagerBase
{
public:

  typedef std::vector<UAKGQueryManagerBase*>  Storage;
  typedef std::vector<UAKGQueryManagerBase*>::iterator  StorageIterator;
  typedef std::vector<UAKGQueryManagerBase*>::const_iterator  
	                                           StorageConstIterator;

protected:
 
  Storage storage_;
  int     storageIndex_;
  GradSoft::RWLock storageRWLock_;

public:

  PoolQueryManager();

  virtual ~PoolQueryManager();


  void init(DBConnectionManagers_impl* dbConnectionManagers_p,
            const DBConnectionParams* params_p
            );

   UAKGQueryManagerBase* getOnePtr();
 
  UAKGQueryBase*  createUAKGQuery(const char* query_text, const char* flags)
		  throw(UAKGQuery2::QueryFlagInvalid,
		        CORBA::SystemException)
  {
   UAKGQueryManagerBase* one=getOnePtr();
   return one->createUAKGQuery(query_text,flags);
   }

  
  CORBA::Long get_next_sequence_value_internal(const char* sequence)
		  throw (UAKGQuery2::QueryProcessingError,
		         UAKGQuery2::QueryFlagInvalid,
		         CORBA::SystemException)
 {
   UAKGQueryManagerBase* one=getOnePtr();
   return one->get_next_sequence_value(sequence);
 }


  UAKGLobBase* createBlobReadingServant()
  {
   getLogger().fatals() << "PoolQueryManager::createBlobReadingServant()" 
	   << std::endl;
   return NULL;
	  // will never called.
  }

  UAKGLobBase* createClobReadingServant()
  {
   getLogger().fatals() << "PoolQueryManager::createClobReadingServant()" 
	   << std::endl;
   return NULL;
	  // will never called.
  }

  UAKGLobBase* createWclobReadingServant()
  {
   getLogger().fatals() << "PoolQueryManager::createWclobReadingServant()" 
	   << std::endl;
   return NULL;
	  // will never called.
  }

  UAKGLobBase* createBlobWritingServant()
  {
   getLogger().fatals() << "PoolQueryManager::createBlobWritingServant()" 
	   << std::endl;
   return NULL;
	  // will never called.
  }

  UAKGLobBase* createClobWritingServant()
  {
   getLogger().fatals() << "PoolQueryManager::createClobWritingServant()" 
	   << std::endl;
   return NULL;
	  // will never called.
  }

  UAKGLobBase* createWclobWritingServant()
  {
   getLogger().fatals() << "PoolQueryManager::createWclobWritingServant()" 
	   << std::endl;
   return NULL;
	  // will never called.
  }

//protected: gcc bug
public:

  virtual char* normalize_sql(const char*);

private:

  PoolQueryManager(const PoolQueryManager&);
  PoolQueryManager& operator=(const PoolQueryManager&);

};

#endif
