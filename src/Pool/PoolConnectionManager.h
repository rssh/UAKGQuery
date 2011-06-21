#ifndef __Pool_PoolConnectionManager_h
#define __Pool_PoolConnectionManager_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif


#ifndef __GradSoft_ProgOptions_h
#include <GradSoft/ProgOptions.h>
#endif


class PoolConnectionManager: public DBConnectionManagerBase
{
public:

  PoolConnectionManager();
  
  void init(GradSoft::ProgOptions& progOptions, GradSoft::Logger& logger,
            PortableServer::POA_ptr poa);

  void close();
  
  bool acceptDBType(const char* db_type)
  {
   return !strncmp(db_type,"Pool",4);  
  }

  UAKGQueryManagerBase* createUAKGQueryManager_(
                  DBConnectionManagers_impl* connectionManagers_p,
				  const DBConnectionParams* params_p);


private:
 
  PoolConnectionManager(const PoolConnectionManager&);
  PoolConnectionManager operator=(const PoolConnectionManager&);


};

void  registerPoolConnectionManager(DBConnectionManagers_impl* managers);

#endif
