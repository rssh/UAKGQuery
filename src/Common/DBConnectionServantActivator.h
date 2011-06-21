#ifndef __Common_DBConnectionServantActivator_h
#define __Common_DBConnectionServantActivator_h

/*
 * part of GradSoft Query Service.
 * (C) Ruslan Shevchenko, 2002,2003
 * (C) Grad-Soft Ltd, Kiev, Ukraine.
 * $Id: DBConnectionServantActivator.h,v 1.4 2003-12-03 19:24:44 rssh Exp $
 */


#ifndef __importCORBA_h
#include <importCORBA.h>
#endif


#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif


class DBConnectionManagers_impl;

/**
*  DBConnection is persistent object.
*    parameters of dbconnections can be restored from object key from connectionsManager.
*
*  DBConnectionServantActivator is responsible for recreating of connection manager servant
*  via parameters.
**/
class DBConnectionServantActivator:

#ifdef ORBACUS

    virtual public POA_PortableServer::ServantActivator

#elif defined(MICO)

    virtual public PortableServer::ServantActivator

#else
#error ORB is not pointed
#endif

{
private:
   DBConnectionManagers_impl*  connectionsManager_p_;
public:

 DBConnectionServantActivator(DBConnectionManagers_impl*  connectionsManager_p);
 ~DBConnectionServantActivator();

 virtual PortableServer::Servant incarnate(
                                  const PortableServer::ObjectId& oid,
                                  PortableServer::POA_ptr         poa
                                 ) 
                                  throw (CORBA::SystemException,
                                         PortableServer::ForwardRequest);                                          

 virtual void etherealize(
                   const PortableServer::ObjectId& oid,
                   PortableServer::POA_ptr         poa,
                   PortableServer::Servant         servant,
                   CORBA::Boolean                  cleanup_in_progress,
                   CORBA::Boolean                  remaining_activations
                          ) throw (CORBA::SystemException);

private:

 DBConnectionServantActivator(const DBConnectionServantActivator&) ;
 DBConnectionServantActivator& operator=(const DBConnectionServantActivator&) ;

};



#endif
