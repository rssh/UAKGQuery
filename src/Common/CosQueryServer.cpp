#ifndef __GradSoft_Logger_h
#include <GradSoft/Logger.h>
#endif

/**
 * part of GradSoft UAKGQuery Service.
 * (C) Grad-Soft Ltd, Kiev, Ukraine.
 * 1998 - 2002
 **/


#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif

#ifdef HAVE_ORACLE
#include <Oracle8/Oracle8ConnectionManager.h>
#endif
#ifdef HAVE_INTERBASE
#include <InterBase/InterBaseConnectionManager.h>
#endif

#ifndef __Pool_PoolConnectionManager_h
#include <Pool/PoolConnectionManager.h>
#endif

#include <GradSoft/ServiceOptions.h>

using namespace GradSoft;
using namespace UAKGQuery2;

void initUAKGQuery2(CORBA::ORB_ptr orb);


DBConnectionManager_ptr createDefaultDBConnectionManager(
                                  CORBA::ORB_ptr orb,
                                  PortableServer::POA_ptr poa, 
                                  ServiceOptions& options,
                                  Logger& logger)
{
  initUAKGQuery2(orb);
                      
  CORBA::ULong nPolicies=2;
  PortableServer::POAManager_var poaManager = poa->the_POAManager();
  poaManager->activate();


  CORBA::PolicyList policyList;
  policyList.length(nPolicies);
  policyList[0]=poa->create_lifespan_policy(PortableServer::PERSISTENT);
  policyList[1]=poa->create_id_assignment_policy(PortableServer::USER_ID);
  const char* serviceName="UAKGQueryService";
  PortableServer::POA_var userPOA = poa->create_POA(serviceName,
	                                   PortableServer::POAManager::_nil(),
                                                 policyList);
  DBConnectionManagers_impl* retval_impl =
                         new DBConnectionManagers_impl(orb, poa, userPOA,
                                                       options,logger);
  poaManager = userPOA->the_POAManager();
  poaManager->activate();

#ifdef HAVE_ORACLE
  registerOracle8ConnectionManager(retval_impl);  
#endif

#ifdef HAVE_INTERBASE
  registerInterBaseConnectionManager(retval_impl);
#endif

  registerPoolConnectionManager(retval_impl);

  PortableServer::ObjectId_var oid = 
            PortableServer::string_to_ObjectId(serviceName);
  userPOA->activate_object_with_id(oid,retval_impl);  


  UAKGQuery2::DBConnectionManager_var retval = retval_impl->_this();
  retval_impl->_remove_ref();
  options.bindServiceObject(orb,retval.in(),retval_impl,serviceName);

  return retval._retn();
}

