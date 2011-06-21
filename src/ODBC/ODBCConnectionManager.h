#ifndef __ODBC_ODBCConnectionManager_h
#define __ODBC_ODBCConnectionManager_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif

#ifndef __GradSoft_sql_INCLUDED
#include <sql.h>
#ifndef  __GradSoft_sql_INCLUDED
#define  __GradSoft_sql_INCLUDED
#endif
#endif                                                         


class ODBCConnectionManager: public DBConnectionManagerBase
{
private:

  SQLHANDLE  envh_;
  bool    initialized_;  
  CORBA::String_var xaOpenString_;

public:

  ODBCConnectionManager();
  
  void init(GradSoft::ProgOptions& progOptions, GradSoft::Logger& logger,
            PortableServer::POA_ptr poa);

  void close();
  
  bool acceptDBType(const char* db_type)
  {
   return !strncmp(db_type,"ODBC",4);  
  }

  UAKGQueryManagerBase* createUAKGQueryManager_(
                  DBConnectionManagers_impl* connectionManagers_p,
				  const DBConnectionParams* params_p);


  SQLHANDLE  getEnvHandle()
                { return envh_; }



protected:

  void throwQueryFlagInvalid(OCIError*);  

private:
 
  ODBCConnectionManager(const ODBCConnectionManager&);
  ODBCConnectionManager operator=(const ODBCConnectionManager&);


};

void  registerODBCConnectionManager(DBConnectionManagers_impl* managers);

#endif
