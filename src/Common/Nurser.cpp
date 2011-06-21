#ifndef __Common_Nurser_h
#include <Common/Nurser.h>
#endif

/*
 * part of Grad-Soft Billing.
 * (C) Grad-Soft Ltd, 2003
 * http://www.gradsoft.kiev.ua
 */

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif

#ifndef __GradSoft_sstream_INCLUDED
#include <sstream>
#ifndef __GradSoft_sstream_INCLUDED
#define __GradSoft_sstream_INCLUDED
#endif
#endif



Nurser::Nurser(DBConnectionManagers_impl* dbConnectionManagers_p)
	:GradSoft::Thread(),
	 quit_(false),
	 quitMutex_(),
	 fatal_(false),
         timeToSleep_(60),
         dbConnectionsManager_p_(dbConnectionManagers_p),
         activitiesMutex_(),
	 activities_()
{
}

void Nurser::init()
{
  ActivityHandler h = &Nurser::backupPersistentConnections;
  addActivity("backupPersistentConnections",60,h);
  h = &Nurser::evictConnections;
  addActivity("evictConnections",60,h);
}


void Nurser::run()
{
 while(!getQuit()) {
  int restTimeToSleep=timeToSleep_;
  while(!getQuit() && restTimeToSleep!=0) {
	  GradSoft::Thread::sleepCurrent(1);
	  --restTimeToSleep;
  }
  time_t now = time(NULL);
  ActivitiesIterator it=activities_.begin();
  for(;;)
  {
    {
     GradSoft::MutexLocker ml(activitiesMutex_);
     if (it==activities_.end()) break;
    }
    if (getQuit()) break;
    if ((now - (*it).lastTouch) >= (*it).timeToSleep) {
      try {
        (this->*((*it).handler))(*it);
      }catch(const ActivityException& ex){
        if (ex.fatal) {
          GradSoft::Logger::FatalLocker l(getLogger().fatals());
          if (dbConnectionsManager_p_->getDebugLevel() > 5) {
            getLogger().fatals() << "[" << __FILE__ << ":" << __LINE__ << "]"; 
          }

          getLogger().fatals() << ex.message << std::endl; 
        }else{
          GradSoft::Logger::ErrorLocker l(getLogger().errors());
          if (dbConnectionsManager_p_->getDebugLevel() > 5) {
            getLogger().errors() << "[" << __FILE__ << ":" << __LINE__ << "]"; 
          }
          getLogger().errors() << ex.message << std::endl; 
        }
        if (ex.quitNurser) {
          setQuit(true);
        }
        if (ex.fatal) {
          setFatal(true);
        }
      }catch(const std::exception& ex){
        {
          GradSoft::Logger::FatalLocker l(getLogger().fatals());
          if (dbConnectionsManager_p_->getDebugLevel() > 5) {
            getLogger().fatals() << "[" << __FILE__ << ":" << __LINE__ << "]"; 
          }
          getLogger().fatals() << "exception in background action "; 
          getLogger().fatals() << (*it).name << ":" << ex.what();
          getLogger().fatals() << std::endl;
        }
        setQuit(true);
        setFatal(true);
      }catch(...){
        {
          GradSoft::Logger::FatalLocker l(getLogger().fatals());
          if (dbConnectionsManager_p_->getDebugLevel() > 5) {
            getLogger().fatals() << "[" << __FILE__ << ":" << __LINE__ << "]"; 
          }
          getLogger().fatals() << 
		      "unhandled exception in background action "; 
          getLogger().fatals() << (*it).name << std::endl;
        }
        setQuit(true);
        setFatal(true);
      }
      (*it).lastTouch=now;
      if (getQuit()) {
        break;
      }		
    }
    {
    GradSoft::MutexLocker ml(activitiesMutex_);
     ++it;
    }
  } /* end of for loop of activities*/
 } /* end while(!isQuit()) loop */
}


void Nurser::addActivity(const char* name, int timeToSleep, 
		         Nurser::ActivityHandler handler)
{
 GradSoft::MutexLocker ml(activitiesMutex_);
 activities_.push_back(Activity(name,timeToSleep,handler));
}


void Nurser::backupPersistentConnections(Activity& activity)
{
 time_t connectionsLastTouch = dbConnectionsManager_p_->getConnectionsLastTouch();  
 if (connectionsLastTouch >= activity.lastTouch) {
  try {
   dbConnectionsManager_p_->backupConnections();
  }catch(DBConnectionManagers_impl::FileIOException& ex){
    GradSoft::Logger::ErrorLocker l(getLogger().errors());
    if (dbConnectionsManager_p_->getDebugLevel() > 5) {
      getLogger().errors() << "[" << __FILE__ << ":" << __LINE__ << "] ";  
    }
    getLogger().errors() << ex.what() << std::endl; 
    throw ActivityException(ex.what(),true,true);
  }
 }
}


void Nurser::evictConnections(Activity& activity)
{
 try {
  dbConnectionsManager_p_->evictConnections();
 }catch(const CORBA::SystemException& ex){
  std::ostringstream omessage;
  omessage << ex ;
  throw ActivityException(omessage.str(),false,false);
 }catch(const std::runtime_error& ex){
  throw ActivityException(ex.what(),false,false);
 }
}

GradSoft::Logger&  Nurser::getLogger()
{
   return dbConnectionsManager_p_->getLogger();
}


/*
 *  struct Activity
 *  {
 *   std::string name;
 *   time_t      lastTouch;
 *   int         timeToSleep;
 *   ActivityHandler handler;
 */
  
Nurser::Activity::Activity(const char* theName, 
	                   int theTimeToSleep, 
	                   Nurser::ActivityHandler theHandler)
  :name(theName),
   lastTouch(time(NULL)),
   timeToSleep(theTimeToSleep),
   handler(theHandler)
{
}
     
