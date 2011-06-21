#ifndef __Oracle8_OCIHandlers_h
#define __Oracle8_OCIHandlers_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __GradSoft_Mutex_h
#include <GradSoft/Mutex.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __Oracle8_importOCI_h
#include <Oracle8/importOCI.h>
#endif


#include <deque>

#ifndef assert
#include <assert.h>
#endif

/*
#ifdef HAVE_NAMESPACES
using namespace GradSoft;
#ifndef __STD_USED
using namespace std;
#define __STD_USED
#endif
#endif
*/

/*
 alloc modes switch
*/

#define OB_XA_ALLOC_MODE        0x0001
#define NEW_CONTEXT_ALLOC_MODE  0x0002
#define NEW_ENV_ALLOC_MODE      0x0004

class SessionLevelOCIHandlersPool;

struct AllocHandleException 
{ 
 sword     code;
 OCIError* ociError;
 AllocHandleException(sword _code, OCIError* _ociError)
	 :code(_code),ociError(_ociError) {}
};

struct SessionBeginException 
{ 
  sword code; 
  OCIError* ociError;
  SessionBeginException(sword _code, OCIError* _ociError)
    :code(_code),
     ociError(_ociError)
  {}
};

struct TransactionJoinException
{
 const char* msg;
 TransactionJoinException(const char* errmsg)
  :msg(errmsg) {}
};

struct FreeHandleException { sword code; FreeHandleException(sword _code):code(_code){}};

class Oracle8QueryManager;

class SessionLevelOCIHandlersPool;

/**
 * this class keep set of session-level Oracle connection handlers.
 * (one Set per one Query)
 **/
class SessionLevelOCIHandlersSet
{
private:
  
  // general description:
  //   handlers can be allocated by constructor, or get from proto.
  //   if xxxRefCount == -1 => xxx is getted from proto, otherwise
  //   allocated there.
  //    (proto==NULL => all handlers allocated there)
  //  
  // note, that we not need mutex for refCounts, becouse constructors and
  //  destructors are colled in section, protected by storageMutex_ of
  //  OCISessionLevelOCIHandlersPool_

  OCIEnv*      ociEnv_p_;
  OCIError*    ociError_p_; 
  OCISvcCtx*   ociSvcCtx_p_; 
  OCIServer*   ociServer_p_;
  OCISession*  ociSession_p_;

  SessionLevelOCIHandlersSet* proto_p_;
  GradSoft::Mutex             mutex_;

  int envRefCount_;
  int errorRefCount_;
  int serverRefCount_;
  int svcCtxRefCount_;
  int sessionRefCount_;

  Oracle8QueryManager*  queryManager_p_;
  SessionLevelOCIHandlersPool* pool_p_;

private:

  void  incRefCount(int& refCount,void (SessionLevelOCIHandlersSet::*inc)() );

  // returns true if some destrucotr was called
  bool  decRefCount(int& refCount, bool (SessionLevelOCIHandlersSet::*dec)());

  // return true, if destructor was called
  bool  checkDestruction();

  SessionLevelOCIHandlersSet(
                            Oracle8QueryManager* queryManager_p,
                            CORBA::ULong mode,
			    SessionLevelOCIHandlersPool* pool,
                            SessionLevelOCIHandlersSet* proto,
                            OCIEnv* ociEnv_p = NULL, 
                            OCIError*  ociError_p = NULL,
                            OCISvcCtx* ociSvcCtx_p = NULL,
                            OCIServer* ociServer_p = NULL, 
                            OCISession* ociSession_p = NULL
                            );

  ~SessionLevelOCIHandlersSet();

  void ociHandleAlloc(dvoid* parent_p, dvoid** handler_pp, ub4 type);

public:

  static SessionLevelOCIHandlersSet* create(
                            Oracle8QueryManager* queryManager_p,
                            CORBA::ULong mode,
                            SessionLevelOCIHandlersPool* pool,
                            SessionLevelOCIHandlersSet* proto = NULL,
                            OCIEnv* ociEnv_p = NULL, 
                            OCIError*  ociError_p = NULL,
                            OCISvcCtx* ociSvcCtx_p = NULL,
                            OCIServer* ociServer_p = NULL, 
                            OCISession* ociSession_p = NULL
                            )
   {
    return new SessionLevelOCIHandlersSet(queryManager_p,mode,pool,proto,
                                   ociEnv_p,ociError_p,ociSvcCtx_p,
                                   ociServer_p,ociSession_p); 
  }

  bool decUsage();

  void incEnvRefCount() ;
  // returns true if some underl;aying destructor was calles
  bool decEnvRefCount() ;
  bool envAllocated() const { return envRefCount_ != -1; }

  void incErrorRefCount() ;
  bool decErrorRefCount() ;
  bool errorAllocated() const { return errorRefCount_ != -1; }

  void incServerRefCount() ;
  bool decServerRefCount() ;
  bool serverAllocated() const { return serverRefCount_ != -1; }

  void incSvcCtxRefCount() ;
  bool decSvcCtxRefCount() ;
  bool svcCtxAllocated() const { return svcCtxRefCount_ != -1; }

  void incSessionRefCount() ;
  bool decSessionRefCount() ;
  bool sessionAllocated() const { return sessionRefCount_ != -1; }

  SessionLevelOCIHandlersSet* getProtoPtr()
  { GradSoft::MutexLocker ml(mutex_); return proto_p_; }

  void setProtoPtr(SessionLevelOCIHandlersSet* newProto_p)
  { GradSoft::MutexLocker ml(mutex_); proto_p_=newProto_p; }


public:

  OCIEnv*        getEnv_p()        { return ociEnv_p_; }
  OCIError*      getError_p()      { return ociError_p_; }
  OCISvcCtx*     getSvcCtx_p()     { return ociSvcCtx_p_; }
  OCIServer*     getServer_p()     { return ociServer_p_; }
  OCISession*    getSession_p()    { return ociSession_p_; }

private:

  SessionLevelOCIHandlersSet(const SessionLevelOCIHandlersSet&);
  SessionLevelOCIHandlersSet& operator=(const SessionLevelOCIHandlersSet&);

};

class SessionLevelOCIHandlersPool
{
private:

   typedef std::deque<SessionLevelOCIHandlersSet*> StorageType;
   typedef std::deque<SessionLevelOCIHandlersSet*>::iterator StorageIteratorType;


   StorageType storage_;
   GradSoft::Mutex       storageLock_; 

   SessionLevelOCIHandlersSet* pFirst_;
   SessionLevelOCIHandlersSet* pLast_;

   Oracle8QueryManager*  queryManager_p_;

public:

   SessionLevelOCIHandlersPool(Oracle8QueryManager* queryManager_p)
    :storage_(),storageLock_(),
     pFirst_(NULL),pLast_(NULL),
     queryManager_p_(queryManager_p)
   {
	 assert(queryManager_p!=NULL);
   }

   ~SessionLevelOCIHandlersPool();
 

   SessionLevelOCIHandlersSet*  alloc(CORBA::ULong mode);

   void dealloc(SessionLevelOCIHandlersSet* hndls);

   void substProto(SessionLevelOCIHandlersSet* fromProto, 
                   SessionLevelOCIHandlersSet* toProto);

   OCIEnv*        getEnv_p();
   OCIError*      getError_p();
   OCISvcCtx*     getSvcCtx_p(); 

private:

   SessionLevelOCIHandlersPool(const SessionLevelOCIHandlersPool&);
   SessionLevelOCIHandlersPool& operator=(const SessionLevelOCIHandlersPool&);

};

#endif
