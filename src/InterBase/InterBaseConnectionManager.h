#ifndef __InterBase_InterBaseConnectionManager_h
#define __InterBase_InterBaseConnectionManager_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif

#ifndef __GradSoft_ProgOptions_h
#include <GradSoft/ProgOptions.h>
#endif

class InterBaseConnectionManager: public DBConnectionManagerBase
{
private:

  bool    initialized_;
  bool    xa_;
  CORBA::String_var xaOpenString_;


public:

  InterBaseConnectionManager();
  
  void init(GradSoft::ProgOptions& progOptions, GradSoft::Logger& logger,
            PortableServer::POA_ptr poa);

  void close();
  
  bool acceptDBType(const char* db_type)
  {
   return !strcmp(db_type,"InterBase");  
  }

  UAKGQueryManagerBase* createUAKGQueryManager_(
                  DBConnectionManagers_impl* connectionManagers_p,
                  const char* login, const char* passwd,
                  const char* db_name, const char* db_type, const char* other);

private:
 
  InterBaseConnectionManager(const InterBaseConnectionManager&);
  InterBaseConnectionManager operator=(const InterBaseConnectionManager&);


};

void  registerInterBaseConnectionManager(DBConnectionManagers_impl* managers);

#endif
