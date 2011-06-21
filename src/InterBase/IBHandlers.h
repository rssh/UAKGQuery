#ifndef __InterBase_IBHandlers_h
#define __InterBase_IBHandlers_h

/*
 * part of UAKG Query Service
 * (C) GradSoft, Ukraine 2000, 2001
 * http://www.gradsoft.com.ua
 * $Id: IBHandlers.h,v 1.12 2001-12-27 09:54:02 yad Exp $
 */

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __InterBase_importIBASE_h
#include <InterBase/importIBASE.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#include <deque>



struct IBSessionBeginException 
{ 
  ISC_STATUS* ibError;
  // TODO:
  //  change this to string 
  char msg[256];
  IBSessionBeginException(ISC_STATUS* theIBError)
  {
    ibError = theIBError;
  }
  IBSessionBeginException(const char* message)
  {
    strncpy(msg, message, 256);
  }  
};

class InterBaseQueryManager;

class SessionLevelIBHandlersPool;

class SessionLevelIBHandlersSet
{
private:
  
 isc_db_handle  dbHandle_;
 bool		dbHandleAllocated_;
 char*          dpbBuffer_;
 char*          dpb_copy_;
 bool           dpbBufferAllocated_;
 short          dpbLength_;
 ISC_STATUS*    iscStatusVector_;
 bool           iscStatusVectorAllocated_;
 isc_tr_handle  trans_handle_;

 InterBaseQueryManager*  queryManager_p_;

protected:

  friend SessionLevelIBHandlersPool;

  SessionLevelIBHandlersSet(
                            InterBaseQueryManager* queryManager_p,
                            ISC_STATUS* iscStatusVector = NULL,
                            char* dpbBuffer = NULL,
                            short dpbLength = 0,
                            isc_db_handle        dbHandle=0L
                            );

  ~SessionLevelIBHandlersSet();

public:

  isc_db_handle   db_handle()  { return dbHandle_; }
  ISC_STATUS*     status() { return iscStatusVector_; }
  char*           dpb_buffer() { return dpbBuffer_; }
  short           dpb_buffer_length() { return dpbLength_; }
  isc_tr_handle*  trans_handle_p() { return &trans_handle_; }

   static void check_error(ISC_STATUS* status, const char* operation);

private:

  SessionLevelIBHandlersSet(const SessionLevelIBHandlersSet&);
  SessionLevelIBHandlersSet& operator=(const SessionLevelIBHandlersSet&);

};

class SessionLevelIBHandlersPool
{
private:

   typedef std::deque<SessionLevelIBHandlersSet*> StorageType;
   typedef std::deque<SessionLevelIBHandlersSet*>::iterator StorageIteratorType;

   StorageType storage_;
   InterBaseQueryManager*  queryManager_p_;

public:

   SessionLevelIBHandlersPool(InterBaseQueryManager* queryManager_p)
    :storage_(),
     queryManager_p_(queryManager_p)
   {
    assert(queryManager_p!=NULL);
   }

   ~SessionLevelIBHandlersPool();
 

   SessionLevelIBHandlersSet*  alloc(CORBA::ULong mode);

   void dealloc(SessionLevelIBHandlersSet* hndls);

private:

   SessionLevelIBHandlersPool(const SessionLevelIBHandlersPool&);
   SessionLevelIBHandlersPool& operator=(const SessionLevelIBHandlersPool&);


};

#endif
