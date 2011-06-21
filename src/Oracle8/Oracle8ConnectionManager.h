#ifndef __Oracle8_Oracle8ConnectionManager_h
#define __Oracle8_Oracle8ConnectionManager_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif

#ifndef __Oracle8_importOCI_h
#include <Oracle8/importOCI.h>
#endif

#ifndef __GradSoft_ProgOptions_h
#include <GradSoft/ProgOptions.h>
#endif

class Oracle8ConnectionManager: public DBConnectionManagerBase
{
private:

  
  bool    initialized_;
  bool    xa_;
  CORBA::String_var xaOpenString_;

public:

  Oracle8ConnectionManager();
  
  void init(GradSoft::ProgOptions& progOptions, GradSoft::Logger& logger,
            PortableServer::POA_ptr poa);

  void close();
  
  bool acceptDBType(const char* db_type)
  {
   return !strncmp(db_type,"Oracle",6);  
  }

  UAKGQueryManagerBase* createUAKGQueryManager_(
                  DBConnectionManagers_impl* connectionManagers_p,
				  const DBConnectionParams* params_p);


protected:

  void throwQueryFlagInvalid(OCIError*);  

private:
 
  Oracle8ConnectionManager(const Oracle8ConnectionManager&);
  Oracle8ConnectionManager operator=(const Oracle8ConnectionManager&);


};

void  registerOracle8ConnectionManager(DBConnectionManagers_impl* managers);

#endif
