/*
 * part of GradSoft Query Service.
 * (C) Ruslan Shevchenko, 2002, 2003
 * (C) Grad-Soft Ltd, Kiev, Ukraine.
 * $Id: DBConnectionServantActivator.cpp,v 1.10 2007-12-26 13:44:56 rssh Exp $
 */

#ifndef __Common_DBConnectionServantActivator_h
#include <Common/DBConnectionServantActivator.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif



DBConnectionServantActivator::DBConnectionServantActivator(
		 DBConnectionManagers_impl*  connectionsManager_p)
    :connectionsManager_p_(connectionsManager_p)
{
 connectionsManager_p_->_add_ref();
}

DBConnectionServantActivator::~DBConnectionServantActivator()
{
 connectionsManager_p_->_remove_ref();
}


PortableServer::Servant DBConnectionServantActivator::incarnate(
                                  const PortableServer::ObjectId& oid,
                                  PortableServer::POA_ptr         poa
                                 ) 
                                  throw (CORBA::SystemException,
                                         PortableServer::ForwardRequest)
{
 DBConnectionManagers_impl::ConnectionRecord* connectionRecordPtr=NULL;
 DBConnectionParams* connectionParams_p=NULL;
 CORBA::String_var soid;
 try {
	 soid=PortableServer::ObjectId_to_string(oid);
 }catch(const CORBA::BAD_PARAM& ex){
	 GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
         if(connectionsManager_p_->getDebugLevel()>5){
	    connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 
         }
         
	 connectionsManager_p_->getLogger().warnings() << "received invalid object id of db connection" << std::endl;
	 throw CORBA::BAD_PARAM();
 }
 try{
	 connectionRecordPtr = connectionsManager_p_->findConnectionRecordPtr(soid.in());
 }catch(const DBConnectionManagers_impl::ConnectionRecordNotFound& ex){
	 GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
         if(connectionsManager_p_->getDebugLevel()>5){
	    connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 
         }
	 connectionsManager_p_->getLogger().warnings() << "received unregistered object id of db connection:" << soid.in() << std::endl;
	 throw CORBA::OBJECT_NOT_EXIST();
 }catch(const std::invalid_argument& ex){
    GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
    if(connectionsManager_p_->getDebugLevel()>5){
	    connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 
    }
    connectionsManager_p_->getLogger().warnings() << ex.what() << std::endl;
    throw CORBA::OBJECT_NOT_EXIST();
 }
 try {
   connectionRecordPtr->second=connectionsManager_p_->createUAKGQueryManager_(&(connectionRecordPtr->first));
   {
    GradSoft::Logger::InfoLocker l(connectionsManager_p_->getLogger().infos());
    connectionsManager_p_->getLogger().infos() << 
	    " incarnating connection " << soid.in() << std::endl;
   }
   connectionRecordPtr->second->_add_ref();
   return connectionRecordPtr->second;
 }catch(const UAKGQuery2::QueryProcessingError& ex){
   GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
   if(connectionsManager_p_->getDebugLevel()>5){
      connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 

   }
   connectionsManager_p_->getLogger().warnings() << ex.why << std::endl;
   throw CORBA::OBJ_ADAPTER();
 }catch(const CORBA::SystemException& ex){
   GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
   if(connectionsManager_p_->getDebugLevel()>5){
      connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 
   }
   connectionsManager_p_->getLogger().warnings() << "exception during creation of query manager:" << ex  << std::endl;
 }catch(const std::exception& ex){
    GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
    if(connectionsManager_p_->getDebugLevel()>5){
      connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 
    }
    connectionsManager_p_->getLogger().warnings() << ex.what() << std::endl;
   throw CORBA::OBJ_ADAPTER();
 }catch(...){
    GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
    if(connectionsManager_p_->getDebugLevel()>5){
      connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 
    }
    connectionsManager_p_->getLogger().warnings() << "unknown exception during creation of query manager" << std::endl;
   throw CORBA::OBJ_ADAPTER();
 }
}


void DBConnectionServantActivator::etherealize(
                   const PortableServer::ObjectId& oid,
                   PortableServer::POA_ptr         poa,
                   PortableServer::Servant         servant,
                   CORBA::Boolean                  cleanup_in_progress,
                   CORBA::Boolean                  remaining_activations
                          ) throw (CORBA::SystemException)
{
 DBConnectionManagers_impl::ConnectionRecord*  connectionRecordPtr;
 CORBA::String_var soid=PortableServer::ObjectId_to_string(oid);
 try {
	connectionRecordPtr = connectionsManager_p_->findConnectionRecordPtr(soid.in());
 }catch(const DBConnectionManagers_impl::ConnectionRecordNotFound& ex){
	 GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
    if(connectionsManager_p_->getDebugLevel()>5){
      connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 
    }
    connectionsManager_p_->getLogger().warnings() << "impossible - etherealize unregistered db connection:" << soid.in() << std::endl;
    return;
 }catch(const std::invalid_argument& ex){
      GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
      if(connectionsManager_p_->getDebugLevel()>5){
         connectionsManager_p_->getLogger().warnings() <<
              "[" << __FILE__ << ":" << __LINE__ << "]" ; 
      }
      connectionsManager_p_->getLogger().warnings() << "etherealize failed:"
	                                            << ex.what() << std::endl;
 }

 // set last touch to parameters.
 connectionRecordPtr->first.setLastTouch(connectionRecordPtr->second->getLastTouch());

 if (remaining_activations==0) {
    if (connectionRecordPtr->second!=NULL) {
      if (connectionRecordPtr->second->isDestroyed()) {
        connectionsManager_p_->removeQMReference(soid.in());
      }else{
        //try {
          connectionRecordPtr->second->destroy();
        //} catch (const CORBA::Exception& ex) {
        //   servant->_remove_ref();
        //   connectionRecordPtr->second=NULL;
        //   GradSoft::Logger::WarningLocker wl(connectionsManager_p_->getLogger().warnings());
        //   if(connectionsManager_p_->getDebugLevel()>5){
        //     connectionsManager_p_->getLogger().warnings() <<
        //      "[" << __FILE__ << ":" << __LINE__ << "]" ; 
        //   }
        //   connectionsManager_p_->getLogger().warnings() << "destroy on etherealize failed:"
	//                                            << ex << std::endl;
        //   connectionRecordPtr->second=NULL;
        //   throw ex;
        //}
        connectionRecordPtr->second=NULL;
      }
    }
    //if (servant->_get_ref()!=0) {
      servant->_remove_ref();
    //}
 }

}





