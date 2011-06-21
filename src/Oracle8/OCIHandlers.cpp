#include <Oracle8/OCIHandlers.h>

#include <Oracle8/Oracle8QueryManager.h>

#include <algorithm>
#include <sstream>

#ifdef CORBA_HAVE_OTS
#include COSTRANSACTIONS_H
#endif


SessionLevelOCIHandlersSet::SessionLevelOCIHandlersSet(
                            Oracle8QueryManager* queryManager_p,
                            CORBA::ULong mode,
                            SessionLevelOCIHandlersPool* pool_p,
                            SessionLevelOCIHandlersSet* proto_p,
                            OCIEnv* ociEnv_p, 
                            OCIError*  ociError_p,
                            OCISvcCtx* ociSvcCtx_p,
                            OCIServer* ociServer_p, 
                            OCISession* ociSession_p
                            )
{
 queryManager_p_ = queryManager_p;
 pool_p_=pool_p;
 proto_p_=proto_p; 
 if (ociEnv_p == NULL) {
#ifdef CORBA_HAVE_OTS
   if ((mode & OB_XA_ALLOC_MODE)) {
   // get current transaction
      CORBA::ORB_ptr orb=queryManager_p->getORB_();
      CORBA::Object_var object = orb->resolve_initial_references("TransactionCurrent");
      CosTransactions::Current_var transCurrent = 
                        CosTransactions::Current::_narrow(object);
      CosTransactions::Control_var control = transCurrent->get_control();
      if (is_nil(control)) {
         throw CORBA::TRANSACTION_REQUIRED();
      }
      const char* dbname = queryManager_p->get_sz_flag("DB") ;
      ociEnv_p_ = xaoEnv((ub1*)dbname);
      envRefCount_=-1;
      if (ociEnv_p_ == NULL) {
         throw TransactionJoinException("xaoEnv failed");
      }
   }else{
#endif
      (void)OCIEnvInit(&ociEnv_p_, OCI_DEFAULT/*OCI_NO_MUTEX*/, (size_t)0 , (dvoid**)NULL);
      envRefCount_=1;
#ifdef CORBA_HAVE_OTS
   }
#endif
 }else{
   envRefCount_=-1;
   ociEnv_p_ = ociEnv_p;
   assert(proto_p!=NULL) ;
   proto_p->incEnvRefCount();
 }
 if (ociError_p == NULL) {
   ociHandleAlloc(ociEnv_p_,(void**)&ociError_p_,OCI_HTYPE_ERROR);
   errorRefCount_ = 1;
 }else{
   errorRefCount_ = -1;
   ociError_p_ = ociError_p;
   assert(proto_p!=NULL) ;
   proto_p->incErrorRefCount();
   assert( (!errorAllocated() && envAllocated())==false );
 } 
 if (ociServer_p == NULL) {
   ociHandleAlloc(ociEnv_p_,(dvoid**)&ociServer_p_,OCI_HTYPE_SERVER);
   serverRefCount_ = 1;
 }else{
   ociServer_p_ = ociServer_p;
   serverRefCount_ = -1;
   assert(proto_p!=NULL) ;
   proto_p->incServerRefCount();
 }
 if ((mode & OB_XA_ALLOC_MODE)!=0) {
#ifdef USE_OB_XA
    const char* dbname = queryManager_p->get_sz_flag("DB") ;
    ociSvcCtx_p_=xaoSvcCtx((ub1*)dbname);
    svcCtxRefCount_=-1;
    if (ociSvcCtx_p_ == NULL) {
      throw TransactionJoinException("xaoSvcCtx failed");
    }
#else
	queryManager_p_->errors() << "Fatal: XA support is not enabled" << std::endl;
    throw CORBA::NO_IMPLEMENT(); 
#endif
 }else if (ociSvcCtx_p == NULL) {
    ociHandleAlloc((dvoid*)ociEnv_p_,(dvoid**)&ociSvcCtx_p_,OCI_HTYPE_SVCCTX);
    svcCtxRefCount_=1;
 }else{
    svcCtxRefCount_= -1;
    ociSvcCtx_p_ = ociSvcCtx_p;
    assert(proto_p!=NULL) ;
    proto_p->incSvcCtxRefCount();
 }
 if (!(mode & OB_XA_ALLOC_MODE) ) {
    if (serverAllocated()) {
      if (ociSession_p == NULL) {
        char* db_link = queryManager_p_->getDBLink_p();
        sword err=OCIServerAttach(ociServer_p_,
                                  ociError_p_,
                                  (text*)db_link,
                                  strlen(db_link),
                                  OCI_DEFAULT);
        if (err!=OCI_SUCCESS){
           throw SessionBeginException(err,ociError_p_);
        }
        const char* dbname = queryManager_p->get_sz_flag("DB") ;
        if (dbname==NULL) dbname="NULL";
        sword rc = OCIAttrSet((dvoid*)ociServer_p_,OCI_HTYPE_SERVER,
                         (text*)dbname, 0,
                         OCI_ATTR_EXTERNAL_NAME,ociError_p_);
        if (rc!=OCI_SUCCESS) {
           throw SessionBeginException(rc,ociError_p_);
        } 
	std::ostringstream ostr;
        ostr << dbname << "-internal" ;
        const char* internal_dbname = ostr.str().c_str();
        OCIAttrSet((dvoid*)ociServer_p_,OCI_HTYPE_SERVER,
                   (text*)internal_dbname, 0,
                    OCI_ATTR_INTERNAL_NAME,ociError_p_);
        if (rc!=OCI_SUCCESS) {
           throw SessionBeginException(rc,ociError_p_);
        } 
        err = OCIAttrSet( (dvoid*)ociSvcCtx_p_, OCI_HTYPE_SVCCTX,
                          (dvoid*)ociServer_p_, (ub4)0,
                          OCI_ATTR_SERVER, ociError_p_);
        if (err != OCI_SUCCESS) {
            throw SessionBeginException(err,ociError_p_);
        }
        ociSession_p_ = NULL;
        ociHandleAlloc(ociEnv_p_, (dvoid**)&ociSession_p_, (ub4)OCI_HTYPE_SESSION);
        char* username = queryManager_p_->getUsername_p();
        OCIAttrSet((dvoid*)ociSession_p_, (ub4)OCI_HTYPE_SESSION,
                             username,(ub4)strlen(username),
                            (ub4) OCI_ATTR_USERNAME, ociError_p_);
        char* password = queryManager_p_->getPassword_p();
        OCIAttrSet((dvoid*)ociSession_p_, (ub4)OCI_HTYPE_SESSION,
                 password,(ub4)strlen(password),
                 (ub4) OCI_ATTR_PASSWORD, ociError_p_);
        sessionRefCount_ = 1;                    
      }else{
         sessionRefCount_ = -1;                    
         ociSession_p_ = ociSession_p;
         assert(proto_p!=NULL) ;
         proto_p->incSessionRefCount();
      }
      if (svcCtxAllocated()) {
         OCIAttrSet(ociSvcCtx_p_,(ub4)OCI_HTYPE_SVCCTX,(dvoid*)ociSession_p_,
                                 (ub4)0,(ub4)OCI_ATTR_SESSION,ociError_p_);
         sword res = OCISessionBegin(ociSvcCtx_p_,ociError_p_,ociSession_p_,
                                     OCI_CRED_RDBMS,(ub4)OCI_DEFAULT);
         if (res!=OCI_SUCCESS) {
             throw SessionBeginException(res,ociError_p_);
         }
      }
    }else{
      // server ! allocated
      sessionRefCount_ = -1;
      assert(proto_p!=NULL);
      proto_p->incSessionRefCount();
    }
  }else{
      // all allocated in OB_XA
      sessionRefCount_ = -1;
  }

} 


SessionLevelOCIHandlersSet::~SessionLevelOCIHandlersSet()
{
  sword res;
  if (svcCtxAllocated()) {
     res=OCISessionEnd(ociSvcCtx_p_,ociError_p_,ociSession_p_,(ub4)OCI_DEFAULT);
     if (res!=OCI_SUCCESS && res!=OCI_SUCCESS_WITH_INFO) {
	     if (queryManager_p_) {
		 queryManager_p_->errors() << "Error, during end of session" << std::endl;
	     }else{
                 std::cerr << "UAKGQuery2: waring: OCISessionEnd returns error" << std::endl;
	     }
     }
  }
  if (sessionAllocated()){
       res=OCIHandleFree((dvoid*)ociSession_p_, OCI_HTYPE_SESSION);
  }
  if (svcCtxAllocated()){
       res=OCIHandleFree((dvoid*)ociSvcCtx_p_, OCI_HTYPE_SVCCTX);
  }

  if (serverAllocated()) {
       OCIServerDetach(ociServer_p_, ociError_p_, OCI_DEFAULT); 
  }
  if (serverAllocated()) {
    res=OCIHandleFree((dvoid*)ociServer_p_, OCI_HTYPE_SERVER);
  }

  if (errorAllocated()){
       res=OCIHandleFree((dvoid*)ociError_p_, OCI_HTYPE_ERROR);
  }
  if (envAllocated()){
    res=OCIHandleFree((dvoid*)ociEnv_p_, OCI_HTYPE_ENV);
  }
  //if (res) throw FreeHandleException(res);
}



void  SessionLevelOCIHandlersSet::incRefCount(int& refCount,void (SessionLevelOCIHandlersSet::*inc)() )
{
 if(refCount==-1 && proto_p_!=NULL) (proto_p_->*inc)(); else ++refCount;
}

bool  SessionLevelOCIHandlersSet::decRefCount(int& refCount, 
                                bool (SessionLevelOCIHandlersSet::*dec)())
{
 if(refCount==-1 && proto_p_!=NULL) {
      if ((proto_p_->*dec)()) {
          proto_p_ = NULL;
	  return checkDestruction();
      }
      //TODO:
      //   (look's like meory leak is fixed.
      //     [ this will destucted when last proto will be destructed]
      //   but needs check
 } else { 
       --refCount; 
       return checkDestruction();
 }
 return false;
}

bool SessionLevelOCIHandlersSet::checkDestruction()
{
 if(envRefCount_<=0 && errorRefCount_<=0 && serverRefCount_<=0 &&
    svcCtxRefCount_<=0 && sessionRefCount_ <=0) {
  if (proto_p_!=NULL) {
     pool_p_->substProto(this,proto_p_);  
  }
  delete this;
  return true;
 }else{ 
  return false;
 }
}

void SessionLevelOCIHandlersSet::incEnvRefCount() 
{ 
  incRefCount(envRefCount_,&SessionLevelOCIHandlersSet::incEnvRefCount); 
}
bool SessionLevelOCIHandlersSet::decEnvRefCount() 
{ 
 return decRefCount(envRefCount_, 
                    &SessionLevelOCIHandlersSet::decEnvRefCount); 
}

void SessionLevelOCIHandlersSet::incServerRefCount() 
{
 incRefCount(serverRefCount_,&SessionLevelOCIHandlersSet::incServerRefCount);
} 

bool SessionLevelOCIHandlersSet::decServerRefCount() 
{
  return decRefCount(serverRefCount_,&SessionLevelOCIHandlersSet::decServerRefCount);
}

void SessionLevelOCIHandlersSet::incSvcCtxRefCount() 
{
 incRefCount(svcCtxRefCount_,&SessionLevelOCIHandlersSet::incSvcCtxRefCount);
}
                  
bool SessionLevelOCIHandlersSet::decSvcCtxRefCount() 
{
 return decRefCount(svcCtxRefCount_,
                    &SessionLevelOCIHandlersSet::decSvcCtxRefCount);
}
       
void SessionLevelOCIHandlersSet::incSessionRefCount()
{
 incRefCount(sessionRefCount_,
              &SessionLevelOCIHandlersSet::incSessionRefCount);
}


bool SessionLevelOCIHandlersSet::decSessionRefCount()
{ 
  return decRefCount(sessionRefCount_,
              &SessionLevelOCIHandlersSet::decSessionRefCount); 
}

void SessionLevelOCIHandlersSet::
        ociHandleAlloc(dvoid* parent_p, dvoid** handler_pp, ub4 type)
{
 sword err=OCIHandleAlloc(parent_p,handler_pp,type,(size_t)0,(dvoid**)NULL);
 if (err!=OCI_SUCCESS) {
   throw AllocHandleException(err,ociError_p_);
 }
}


SessionLevelOCIHandlersSet*  SessionLevelOCIHandlersPool::alloc(CORBA::ULong mode)
{
 GradSoft::MutexLocker ml(storageLock_);
 StorageIteratorType it = storage_.begin();
 SessionLevelOCIHandlersSet* pNew;
 if (storage_.size()==0) { 
	 if (pFirst_!=NULL) {
		 pNew = pFirst_;
	 }else{
                pNew = SessionLevelOCIHandlersSet::create(queryManager_p_,mode,this);
                pFirst_ = pNew;
	        pLast_ = pNew;
	 }
 } else {
   // TODO:
   //   write some policies which depend from flag. 
   //   for now we implement only: 
   //          - policy whith one connection, with empty flag
   //          - policy whith XA-depend connection with OB_XA_ALLOC_POLICY set
   //          - policy with new environment
   //          - policy with new service context
   //
   SessionLevelOCIHandlersSet* pPrev = pLast_;

   if (mode == NEW_ENV_ALLOC_MODE) {
       pNew = SessionLevelOCIHandlersSet::create(
                                          queryManager_p_,
                                          mode,
					  this,
                                          pPrev, 
                                          NULL,
                                          NULL, 
                                          NULL,
                                          NULL,
                                          NULL
                                            );
   }else if (mode == NEW_CONTEXT_ALLOC_MODE) {
       pNew = SessionLevelOCIHandlersSet::create(
                                          queryManager_p_,
                                          mode,
					  this,
                                          pPrev,
                                          pPrev->getEnv_p(),
                                          NULL, 
                                          NULL,
                                          NULL,
                                          NULL
                                            );
   }else{
       pNew = SessionLevelOCIHandlersSet::create(
                                          queryManager_p_,
                                          mode,
					  this,
                                          pPrev, 
                                          pPrev->getEnv_p(),
                                          pPrev->getError_p(),
                                          pPrev->getSvcCtx_p(),
                                          pPrev->getServer_p(),
                                          pPrev->getSession_p()
                                         );
  }
 }
 it = storage_.insert(it,pNew);
 pLast_=pNew;
 return pNew;
}


bool SessionLevelOCIHandlersSet::decUsage()
{
 if (decEnvRefCount()) return true;
 if (decErrorRefCount()) return true;
 if (decSvcCtxRefCount()) return true;
 if (decServerRefCount()) return true;
 if (decSessionRefCount()) return true;
 return false;
}



void SessionLevelOCIHandlersSet::incErrorRefCount() 
{ 
  incRefCount(errorRefCount_,
              &SessionLevelOCIHandlersSet::incErrorRefCount); }

bool SessionLevelOCIHandlersSet::decErrorRefCount() 
{ 
 return decRefCount(errorRefCount_,
                    &SessionLevelOCIHandlersSet::decErrorRefCount); 
}
                   

void SessionLevelOCIHandlersPool::dealloc(SessionLevelOCIHandlersSet* hndls_p)
{
 GradSoft::MutexLocker ml(storageLock_);
 StorageIteratorType it = std::find(storage_.begin(),storage_.end(),hndls_p);
 if (it==storage_.end()) {
	 queryManager_p_->errors() << "Internal Error: deallocated unallocated handler" << std::endl;
 }else{
   storage_.erase(it);
   if (storage_.size()==0) {
     pFirst_=NULL; pLast_=NULL;
   }else{
     pFirst_=*(storage_.begin());
     pLast_=*(storage_.rbegin());
   }
 }
 hndls_p->decUsage();
}

SessionLevelOCIHandlersPool::~SessionLevelOCIHandlersPool()
{
 GradSoft::MutexLocker ml(storageLock_);

 StorageIteratorType it;
 for (it = storage_.begin(); it!=storage_.end(); ++it) {
   (*it)->decUsage();
 }
 storage_.erase(storage_.begin(),storage_.end());
}


OCIEnv*        SessionLevelOCIHandlersPool::getEnv_p()
{
  if (pLast_==NULL) {
	  GradSoft::MutexLocker ml(storageLock_);
	  pFirst_=SessionLevelOCIHandlersSet::create(queryManager_p_,0,this);
	  pLast_=pFirst_;
  }
  return pFirst_->getEnv_p();
}

OCIError*      SessionLevelOCIHandlersPool::getError_p()
{
  if (pFirst_==NULL) {
	  GradSoft::MutexLocker ml(storageLock_);
	  pFirst_=SessionLevelOCIHandlersSet::create(queryManager_p_,0,this);
	  pLast_=pFirst_;
  }
  return pFirst_->getError_p();
}

OCISvcCtx*     SessionLevelOCIHandlersPool::getSvcCtx_p()
{
  if (pFirst_==NULL) {
     GradSoft::MutexLocker ml(storageLock_);
     pFirst_=SessionLevelOCIHandlersSet::create(queryManager_p_,0,this);
     pLast_=pFirst_;
  }
  return pFirst_->getSvcCtx_p();
}


void SessionLevelOCIHandlersPool::substProto(
		SessionLevelOCIHandlersSet* fromProto_p,
                SessionLevelOCIHandlersSet* toProto_p
		                            )
{
	// note - does not lock mutex, becouse it already locked
 StorageIteratorType it;
 for (it = storage_.begin(); it!=storage_.end(); ++it) {
   if ((*it)->getProtoPtr()==fromProto_p) {
     (*it)->setProtoPtr(toProto_p);
   }
 }
}


