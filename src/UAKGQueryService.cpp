#include <CosQueryPostConfigV2.h>

/*
 * part of Grad-Soft UAKGQuery.
 * (C) Grad-Soft, 1998 - 2002
 * $Id: UAKGQueryService.cpp,v 1.35 2008-05-29 07:22:26 rssh Exp $
 */

#include <importCORBA.h>
#include <importUAKGQuery2.h>

#ifdef CORBA_HAVE_OTS
#ifdef ORBACUS
#include <OB/OTS.h>
#endif
#ifdef USE_OB_XA
#include <OB/OTSXA.h>
#endif
#endif

#include <iostream>

#include <CosQuery2Server.h>
#include <RecordSet/RecordSetImpl.h>


#include <GradSoft/ServiceOptions.h>
using namespace GradSoft;
using namespace UAKGQuery2;
using namespace std;

void putAdditionalOptions(ProgOptions& options)
{
#ifdef USE_OB_XA
 options.put("ORACLE_XA","use Oracle XA string",true);
#endif
 options.put("log-to-stderr","dup log otuput to standart output",false);
 options.put("log-to-file","dup log otuput to file <argument>",true);
 options.put("log-sql","log all evaluated SQL sentences",false);
 options.put("no-syslog","does not use syslog for logging",false);
 options.put("cb-fname","set file name, where references to persistent connections\n"
             "   are stored to <argument>",true);
 options.put("max-idle-connection-age","max age of idle connection in seconds", true);
 options.put("max-idle-persistent-connection-age","max age of idle persistent connection in seconds", true);
 options.put("infinite-persistent-connection-age","assumed, that persistent connections are infinite", false);
 options.put("debug-level","set debug level (from 0 to 10)", true);
}

// init Logger by values of options.
// return true, if initialization was succesfull, otherwise false.
bool initLogger(ProgOptions& options, Logger& logger)
{
 if (options.is_set("log-to-stderr")) {
    logger.setDuppedToStderr(true);
 }
 if (options.is_set("log-to-file")) {
    const char* fname = options.argument("log-to-file");
    try {
      logger.setOutputFile(fname); 
    }catch(const Logger::IOException& ex) {
      cerr << options.argv()[0] << ": can't open file " << fname << endl;
      cerr << options.argv()[0] << ":" << ex.what() << endl;
      return false;
    }
 }
 if (options.is_set("no-syslog")) {
     logger.setSyslogOutput(false);
 }
 return true;
}

int main(int argc, char** argv)
{

  ServiceOptions options;

 try {  

  options.putServiceName("UAKGQueryService");
  putAdditionalOptions(options);
  if (!options.parse(argc,argv)) {
     return 1;
  }

 }catch(const CORBA::SystemException& ex){
   cerr << "System exception during UAKGQuery Object registration"  << endl;
   cerr << ex << endl;
   return 1;
 }

 Logger logger;
 if (!initLogger(options,logger)) {
  return 1;
 }

 try {  

#ifdef ORBACUS
#ifdef CORBA_HAVE_OTS
   OTSINIT(argc,argv);
#endif
#ifdef USE_OB_XA
   if (options.is_set("ORACLE_XA")) {
     OB::XA::OTSInit(argc,argv);
   }
#endif
#endif

 ProgOptions::ArgsHolder argsHolder;
 argsHolder.takeArgv(options);

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
  CORBA::ORB_var orb = CORBA::ORB_init(argsHolder.argc,argsHolder.argv,CORBA_ORB_INIT_THIRD_ARG);
#else
  CORBA::ORB_var orb = CORBA::ORB_init(argsHolder.argc,argsHolder.argv);
#endif

  CORBA::ValueFactoryBase_var factory = new RecordSetFactoryImpl();
  CORBA::ValueFactoryBase_var oldFactory =
        orb -> register_value_factory("IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0", factory);

  CORBA::Object_var poaObj = orb->resolve_initial_references("RootPOA"); 
  PortableServer::POA_var poa = PortableServer::POA::_narrow(poaObj.in());


  DBConnectionManager_var defaultConnectionManager = 
      createDefaultDBConnectionManager(orb.in(),poa.in(),options,logger); 

  UAKGQuerySingleton::init(orb.in());
  UAKGQuerySingleton::setLogger(logger);

  orb->run();

  orb->destroy();

 }catch(const CORBA::SystemException& ex){
  logger.fatals() << "SystemException from main" << endl;
  cerr << ex << endl;
  logger.fatals() << ex << endl;
  return 1;

 }
 return 0;
}

