
#ifndef __InterBase_InterBaseConnectionManager_h
#include <InterBase/InterBaseConnectionManager.h>
#endif

#ifndef __InterBase_InterBaseQueryManager_h
#include <InterBase/InterBaseQueryManager.h>
#endif

#ifdef USE_OB_XA
#include <OB/OTSXA.h>
#endif

extern CORBA::ORB_ptr myORB;

#ifdef USE_OB_XA
extern xa_switch_t xaosw;
#endif


#include <strstream>
#include <iostream>
#include <assert.h>
using namespace std;
using namespace GradSoft;


InterBaseConnectionManager::InterBaseConnectionManager()
    :initialized_(false),
     xa_(false),
     xaOpenString_(CORBA::string_dup(""))
{
}


void InterBaseConnectionManager::init(ProgOptions& progOptions, Logger& logger,
                                      PortableServer::POA_ptr poa)
{
 assert(initialized_ == false);
 DBConnectionManagerBase::init(progOptions, logger,poa);

#ifdef USE_OB_XA
 char* xaString = NULL;
 ostrstream ostr;
 if (options.is_set("INTERBASE_XA")) {
     ostr << "INTERBASE_XA+" << options.argument("INTERBASE_XA");
     xaString = ostr.str();
     xa_ = true;
     xaOpenString_ = CORBA::string_dup(xaString);
     ostr.rdbuf()->freeze(0);
 }else{
     xa_ = false;
 }

 if (xa_) {
   OB::XA::AddResourceManager(myORB,"","InterBase",&xaosw,xaOpenString_,"",true);
 }
#endif

 initialized_ = true;
}

void InterBaseConnectionManager::close()
{
}
  

UAKGQueryManagerBase* InterBaseConnectionManager::createUAKGQueryManager_(
                  DBConnectionManagers_impl* dbConnectionManagers_p,
                  const char* login, const char* passwd,
                  const char* db_name, const char* db_type, const char* other)
{
 UAKGQueryManagerBase* retval = new InterBaseQueryManager();
 try {
   retval->init(dbConnectionManagers_p,login,passwd,db_name, other);
 }catch(...){
   // TODO : catch exceptions and handle it.
   cerr << "... exception in InterBaseConnectionManager::createUAKGQueryManager_" << endl;
 }
 return retval;
}

void  registerInterBaseConnectionManager(DBConnectionManagers_impl* managers)
{

  InterBaseConnectionManager* newCM = new InterBaseConnectionManager(); 

  managers->registerDBDriver("InterBase",newCM);

}

