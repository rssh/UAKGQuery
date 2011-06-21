
#ifndef __ODBC_ODBCConnectionManager_h
#include <ODBC/ODBCConnectionManager.h>
#endif

#ifndef __ODBC_ODBCQueryManager_h
#include <ODBC/ODBCQueryManager.h>
#endif

#ifdef USE_OB_XA
#include <OB/OTSXA.h>
#endif

#ifndef __UAKGQuerySingleton_h
#include <UAKGQuerySingleton.h>
#endif



#include <strstream>
#include <iostream>
using namespace std;
using namespace GradSoft;

ODBCConnectionManager::ODBCConnectionManager()
    :initialized_(false),
     xaOpenString_(CORBA::string_dup("")),
     envh_(SQL_NULL_HENV)

  {
  }

void ODBCConnectionManager::init(ProgOptions& progOptions, Logger& logger,
                                    PortableServer::POA_ptr poa)
{
 assert(initialized_ == false);
 DBConnectionManagerBase::init(progOptions, logger,poa);

//  SQLAllocHandle
//  SQLSetEnvAttr


 initialized_ = true;
}

void ODBCConnectionManager::close()
{
  SQLFreeHandle(ENV);
}
  

UAKGQueryManagerBase* ODBCConnectionManager::createUAKGQueryManager_(
                  DBConnectionManagers_impl* dbConnectionManagers_p,
				  const DBConnectionParams* params_p)
{
 //throw ::UAKGQuery2::QueryProcessingError("qqq",0,0);
 UAKGQueryManagerBase* retval = new ODBCQueryManager();
 retval->init(dbConnectionManagers_p,params_p);
 return retval;
}





void  ODBCConnectionManager::throwQueryFlagInvalid(SQLSMALLINT handleType, SQLHANDLE handle)
{
  //TODO: use OCIDiag
  throw UAKGQuery2::QueryFlagInvalid("query flag invalid:");
}

void  registerODBCConnectionManager(DBConnectionManagers_impl* managers)
{
  ODBCConnectionManager* newCM = new ODBCConnectionManager(); 
  managers->registerDBDriver("ODBC",newCM,true);
}

