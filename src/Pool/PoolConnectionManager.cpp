
#ifndef __Pool_PoolConnectionManager_h
#include <Pool/PoolConnectionManager.h>
#endif

#ifndef __Pool_PoolQueryManager_h
#include <Pool/PoolQueryManager.h>
#endif

#ifndef __UAKGQuerySingleton_h
#include <UAKGQuerySingleton.h>
#endif



#include <strstream>
#include <iostream>
#include <vector>

PoolConnectionManager::PoolConnectionManager()
  {
  }

void PoolConnectionManager::init(GradSoft::ProgOptions& progOptions, 
		                 GradSoft::Logger& logger,
                                 PortableServer::POA_ptr poa)
{
 DBConnectionManagerBase::init(progOptions, logger,poa);
}

void PoolConnectionManager::close()
{
}
  

UAKGQueryManagerBase* PoolConnectionManager::createUAKGQueryManager_(
                  DBConnectionManagers_impl* dbConnectionManagers_p,
				  const DBConnectionParams* params_p)
{
 UAKGQueryManagerBase* retval = new PoolQueryManager();
 retval->init(dbConnectionManagers_p,params_p);
 return retval;
}


void  registerPoolConnectionManager(DBConnectionManagers_impl* managers)
{
  PoolConnectionManager* newCM = new PoolConnectionManager(); 
  managers->registerDBDriver("Pool",newCM,true);
}

