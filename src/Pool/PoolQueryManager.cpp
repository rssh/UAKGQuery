#include <Pool/PoolQueryManager.h>
#include <Common/UAKGQueryBase.h>
#include <UAKGQuerySingleton.h>



PoolQueryManager::PoolQueryManager()
	:storage_(),
	 storageIndex_(0),
         storageRWLock_()
	 //maxPoolUsage_(0)
{}

PoolQueryManager::~PoolQueryManager()
{
 GradSoft::WriteLocker wl(storageRWLock_);
 for(StorageIterator it=storage_.begin(); it!=storage_.end(); ++it) {
    delete (*it);
    *it=NULL;
 }
 storage_.resize(0);
}

#define __FUNCTION__ "PoolQueryManager::init"
void PoolQueryManager::init( 
                             DBConnectionManagers_impl* dbConnectionManagers_p,
			     const DBConnectionParams* params_p)
{
 UAKGQueryManagerBase::init(dbConnectionManagers_p, params_p);
 int poolSize=flags_.get_int("PoolSize");
 if (poolSize==0) {
   GradSoft::Logger::WarningLocker l(warnings());
   warnings() << "PoolSize is not set, use default: 10" << std::endl;
   poolSize=10;
 }
 const char* dbDriver=flags_.get_sz("DBDriver");
 if (dbDriver==NULL) {
   GradSoft::Logger::WarningLocker l(warnings());
   warnings() << "dbDriver is not set, use default: Oracle" << std::endl;
   dbDriver="Oracle";
 }else if(strcmp(dbDriver,"Pool")==0) {
   throw UAKGQuery2::QueryFlagInvalid("Pool of Pools is not supported");
 }
 DBConnectionManagerBase* mngr=dbConnectionManagers_p->findDriver(dbDriver);
 storage_.resize(poolSize);
 for(int i=0; i<poolSize; ++i) {
    storage_[i]=mngr->createUAKGQueryManager_(dbConnectionManagers_p,params_p);
 }
}
#undef __FUNCTION__


#define __FUNCTION__ "PoolQueryManager::getOnePtr"
UAKGQueryManagerBase* PoolQueryManager::getOnePtr()
{
 if (storage_.size()==0) {
   throw UAKGQuery2::QueryProcessingError("Size of pool is not set",50,0);
 }
 int minNQueries;
 {
  GradSoft::ReadLocker rl(storageRWLock_);
  minNQueries=storage_[storageIndex_]->number_of_queries();
  if (minNQueries==0) {
    return storage_[storageIndex_];
  }
 }
 int nextIndex;
 int minIndex;
 int invNAttempts;
 {
  GradSoft::ReadLocker rl(storageRWLock_);
  nextIndex=storageIndex_;
  invNAttempts=storage_.size();
 }
 minIndex=nextIndex;
 while(minNQueries!=0 && invNAttempts!=0) {
     int nQueries=storage_[nextIndex]->number_of_queries();
     if (nQueries==0) {
       minIndex=nextIndex;
       break;
     }else if (nQueries < minNQueries) {
       minNQueries=nQueries;
       minIndex=nextIndex;
     }
     --invNAttempts;
     nextIndex=((nextIndex+1)%storage_.size()); 
 } 
 UAKGQueryManagerBase* retval;
 {
  GradSoft::WriteLocker rl(storageRWLock_);
  storageIndex_=minIndex;
  retval=storage_[minIndex];
 }
 return retval;
}
#undef __FUNCTION__

char*  PoolQueryManager::normalize_sql(const char* s)
{
 if (storage_.size()==0) {
   throw UAKGQuery2::QueryProcessingError("Size of pool is not set",51,0);
 }
 GradSoft::ReadLocker rl(storageRWLock_);
 return storage_[storageIndex_]->normalize_sql(s);
}

