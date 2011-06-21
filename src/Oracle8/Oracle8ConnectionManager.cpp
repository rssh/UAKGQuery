
#ifndef __Oracle8_Oracle8ConnectionManager_h
#include <Oracle8/Oracle8ConnectionManager.h>
#endif

#ifndef __Oracle8_Oracle8QueryManager_h
#include <Oracle8/Oracle8QueryManager.h>
#endif

#ifdef USE_OB_XA
#include <OB/OTSXA.h>
#endif

#ifndef __UAKGQuerySingleton_h
#include <UAKGQuerySingleton.h>
#endif

#ifdef USE_OB_XA
extern xa_switch_t xaosw;
#endif


#include <strstream>
#include <iostream>
using namespace std;
using namespace GradSoft;

Oracle8ConnectionManager::Oracle8ConnectionManager()
    :initialized_(false),
     xa_(false),
     xaOpenString_(CORBA::string_dup(""))
  {
  }

void Oracle8ConnectionManager::init(ProgOptions& progOptions, Logger& logger,
                                    PortableServer::POA_ptr poa)
{
 assert(initialized_ == false);
 DBConnectionManagerBase::init(progOptions, logger,poa);

#ifdef USE_OB_XA
 char* xaString = NULL;
 ostrstream ostr;
 if (options.is_set("ORACLE_XA")) {
     ostr << "ORACLE_XA+" << options.argument("ORACLE_XA");
     xaString = ostr.str();
     xa_ = true;
     xaOpenString_ = CORBA::string_dup(xaString);
     ostr.rdbuf()->freeze(0);
 }else{
     xa_ = false;
 }

 if (xa_) {
   OB::XA::AddResourceManager(UAKGQuerySingleton::getORB_ptr(),"",
                              "Oracle",&xaosw,xaOpenString_,"",true);
 }
#endif
#define OCIInitializeModeFLAGS (OCI_THREADED|OCI_OBJECT)
//|OCI_OBJECT)
 OCIInitialize((ub4)(OCIInitializeModeFLAGS),
               (dvoid*)NULL,
               /*((dvoid*)(*)())*/NULL,
               /*((dvoid*)(*)())*/NULL,
               /*((dvoid*)(*)())*/NULL
              );
 initialized_ = true;
}

void Oracle8ConnectionManager::close()
{
}
  

UAKGQueryManagerBase* Oracle8ConnectionManager::createUAKGQueryManager_(
                  DBConnectionManagers_impl* dbConnectionManagers_p,
				  const DBConnectionParams* params_p)
{
 //throw ::UAKGQuery2::QueryProcessingError("qqq",0,0);
 if (dbConnectionManagers_p->getDebugLevel() >= 3) {
   params_p->print(getLogger().errors());
   getLogger().errors() << std::endl; 
 }
 UAKGQueryManagerBase* retval = new Oracle8QueryManager();
 retval->init(dbConnectionManagers_p,params_p);
 return retval;
}





void  Oracle8ConnectionManager::throwQueryFlagInvalid(OCIError* ociError_p)
{
  const char* msg;
  if (ociError_p==NULL) {
    msg="Unknown error";
  }else{
    text buff[512];
    sb4 code;
    (void)OCIErrorGet((dvoid*)ociError_p,(ub4)1,(text*)NULL,
                     &code,buff, (ub4)sizeof(buff), OCI_HTYPE_ERROR);
    msg=(char*)buff;
  }
  {
    Logger::ErrorLocker l(errors());
    errors() << "QueryFlagInvalid:" << msg << endl;
  }
  throw UAKGQuery2::QueryFlagInvalid(msg);
}

void  registerOracle8ConnectionManager(DBConnectionManagers_impl* managers)
{
  Oracle8ConnectionManager* newCM = new Oracle8ConnectionManager(); 
  managers->registerDBDriver("Oracle8",newCM,true);
  managers->registerDBDriver("Oracle9",newCM,false);
  managers->registerDBDriver("Oracle", newCM,false);
}

