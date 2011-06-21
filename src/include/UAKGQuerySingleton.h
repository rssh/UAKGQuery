#ifndef __UAKGQuerySingleton_h
#define __UAKGQuerySingleton_h

/*
 * part of GradSoft UAKGQuery Service
 * (C) GradSoft, Kiev, Ukraine 1998-2008
 * http://www.gradsoft.com.ua
 * $Id: UAKGQuerySingleton.h,v 1.5 2008-05-29 07:22:27 rssh Exp $
 */

#ifndef __CosQueryPostConfig_h
#include <CosQueryPostConfig.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __GradSoft_Logger_h
#include <GradSoft/Logger.h>
#endif

#include <iostream>

class UAKGQuerySingleton
{
private:

 static UAKGQuerySingleton singleton_;
 CORBA::ORB_ptr                   orb_;
 PortableServer::Current_ptr curr_;
 GradSoft::Logger*           logger_ptr_;
 bool                        logger_ownity_;

 UAKGQuerySingleton()
  :orb_(CORBA::ORB::_nil()), curr_(PortableServer::Current::_nil()),
   logger_ptr_(NULL),logger_ownity_(false)
 {}

 ~UAKGQuerySingleton()
 {
  if (logger_ownity_ && logger_ptr_!=NULL) {
     delete logger_ptr_;
     logger_ownity_=false;
  }
 }

public:

 static  void init(CORBA::ORB_ptr orb)
 {
   singleton_.orb_ = orb;
 }

 static CORBA::ORB_ptr getORB_ptr()
 {
   if (CORBA::is_nil(singleton_.orb_)) {
     std::cerr << "Fatal: UAKGQueryService was not initialized" << std::endl;
   }
   return singleton_.orb_;
 }

 static PortableServer::Current_ptr getCurrent_ptr()
 {
   if (CORBA::is_nil(singleton_.orb_)) {
     std::cerr << "Fatal: UAKGQueryService was not initialized" << std::endl;
   }
   if (CORBA::is_nil(singleton_.curr_)) {
       CORBA::Object_var obj =
               singleton_.orb_->resolve_initial_references("POACurrent");
       singleton_.curr_ = PortableServer::Current::_narrow(obj);
   }
   return singleton_.curr_;
 }

 static GradSoft::Logger&  getLogger()
 {
  return singleton_.getLogger_();
 }

 /**
  * set logger.
  *(ownity of logger left in caller)
  **/
 static void setLogger(GradSoft::Logger& logger)
 {
   singleton_.setLogger_(logger);
 }

private:

 GradSoft::Logger&  getLogger_()
 {
   if (logger_ptr_==NULL) {
       logger_ptr_=new GradSoft::Logger();
       logger_ownity_=true;
   }
   return *logger_ptr_;
 }

 void setLogger_(GradSoft::Logger& newLogger)
 {
  if (logger_ptr_!=NULL) {
     if (logger_ownity_) {
       delete logger_ptr_;
     }
  }
  logger_ptr_ = &newLogger;
  logger_ownity_ = false;
 }

};

#endif
