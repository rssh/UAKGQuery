#ifndef __Common_Nurser_h
#define __Common_Nurser_h

#ifndef __GradSoft_Threading_h
#include <GradSoft/Threading.h>
#endif

#ifndef __GradSoft_Logger_h
#include <GradSoft/Logger.h>
#endif

class DBConnectionManagers_impl;

/**
 * Nurser is a background thread, whid do various work,
 *  such as:
 *     - update file of persistent referenced if needed.
 *     - evict long-agne inactive connections.
 *   and so on.
 **/
class Nurser: public GradSoft::Thread
{
public:

  Nurser(DBConnectionManagers_impl* connectionsManager);

  void init();

  void run();

  bool  getFatal() 
              { return fatal_; }

public:

  struct ActivityException
  {
    std::string message;
    bool  quitNurser;
    bool  fatal;

    ActivityException(const std::string& theMessage, 
		      bool theQuitNurser,
		      bool theFatal)
	    :message(theMessage),
             quitNurser(theQuitNurser),
	     fatal(theFatal)
	     {}
  };

  struct Activity;

  typedef void (Nurser::*ActivityHandler)(Activity&);

  struct Activity
  {
    std::string name;
    time_t      lastTouch;
    int         timeToSleep;
    ActivityHandler handler;
    //
    Activity(const char* theName, 
	     int theTimeToSleep, 
	     ActivityHandler theHandler);
  };

public:

  void addActivity(const char* name, int timeToSleep, ActivityHandler handler);

  //
  // activities
  //
 
  void backupPersistentConnections(Activity& activity); 

  void evictConnections(Activity& activity);

private:
  bool            quit_;
  GradSoft::Mutex quitMutex_;

  bool            fatal_;

  /**
   * time to sleep between work
   *   (
   **/
  int             timeToSleep_;

  DBConnectionManagers_impl* dbConnectionsManager_p_;

  typedef std::deque<Activity> Activities;
  typedef Activities::iterator ActivitiesIterator;

  GradSoft::Mutex      activitiesMutex_;
  Activities           activities_;

private:

  bool  getQuit()
  {
   GradSoft::MutexLocker ml(quitMutex_);
   return quit_;
  } 

public:

  void  setQuit(bool v)
  {
   GradSoft::MutexLocker ml(quitMutex_);
   quit_=v;
  }

  void  setFatal(bool v)
  {
   fatal_=v;
  }

private: 

  GradSoft::Logger& getLogger();


};


#endif
