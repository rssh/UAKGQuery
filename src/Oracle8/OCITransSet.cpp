/*
 * part of UAKGQueryService
 *  (C) Ruslan Shevchenko, 1999, 2000
 * $Id: OCITransSet.cpp,v 1.8 2002-01-04 13:11:43 srkr Exp $
 */

#ifndef __Oracle8_OCITransSet_h
#include <Oracle8/OCITransSet.h>
#endif

#ifndef __Common_UAKGQueryUtils_h
#include <Common/UAKGQueryUtils.h>
#endif

#ifdef CORBA_HAVE_OTS

OCITransSet::StorageIndexedByTransaction  OCITransSet::byTransaction_;
OCITransSet::StorageIndexedBySvcCtx       OCITransSet::bySvcCtx_;
GradSoft::Mutex OCITransSet::attachMutex_;


OCITransSet::TransRecord*  OCITransSet::attach(SessionLevelOCIHandlersSet& hs,
                                               const XID&   xid, 
                                               unsigned long timeout,
                                               bool   incUsage)
{
 OCISvcCtx* ociSvcCtx = hs.getSvcCtx_p();
 ByTransactionIterator it = byTransaction_.find(const_cast<XID*>(&xid));
 TransRecord* retval;
 if (it==byTransaction_.end_()) {
   // new transaction
   TransRecord* newTransRecord_p = new TransRecord;
   newTransRecord_p->xid_p = new XID;
   *(newTransRecord_p->xid_p) = xid;
   newTransRecord_p->usageCount = (incUsage ? 1 : 0);
   newTransRecord_p->timeout = timeout;
   newTransRecord_p->state = active;

   OCITrans* ociTrans_p;
   OCIHandleAlloc(
                  hs.getEnv_p(),
                  (dvoid**)&ociTrans_p,
                  OCI_HTYPE_TRANS,
                  0,NULL
              );
   newTransRecord_p->ociTrans_p = ociTrans_p;
   {
    //MutexLocker ml(attachMutex_);
    GradSoft::WriteLocker wl(byTransaction_.get_rwlock());
    sword rc =OCIAttrSet(ociTrans_p,OCI_HTYPE_TRANS,
                 (text*)newTransRecord_p->xid_p, sizeof(XID),
                 OCI_ATTR_XID,
                 hs.getError_p());
    if (rc!=OCI_SUCCESS && rc!=OCI_SUCCESS_WITH_INFO) {
      cleanupTransRecord(newTransRecord_p);
      throw OCIException(hs.getError_p());
    }

    byTransaction_.get_container_()[newTransRecord_p->xid_p]=newTransRecord_p;
    BySvcCtxIterator it1 = bySvcCtx_.find(hs.getSvcCtx_p());
       
    if ( it1 != bySvcCtx_.end_() ) {
      // this service context is associated with other transaction.
      // detach this transaction for now.
      OCITransDetach(hs.getSvcCtx_p(),hs.getError_p(),OCI_DEFAULT);
    }

    rc = OCIAttrSet(hs.getSvcCtx_p(),OCI_HTYPE_SVCCTX,
                   ociTrans_p, (ub4)0,
                   OCI_ATTR_TRANS,
                   hs.getError_p());
    if (rc!=OCI_SUCCESS && rc!=OCI_SUCCESS_WITH_INFO) {
       cleanupByTransaction(xid);
       throw OCIException(hs.getError_p());
    }

    rc = OCITransStart(hs.getSvcCtx_p(),hs.getError_p(),
                       timeout,OCI_TRANS_NEW);

    if (rc!=OCI_SUCCESS && rc!=OCI_SUCCESS_WITH_INFO) {
      cleanupByTransaction(xid);
      throw OCIException(hs.getError_p());
    }

    bySvcCtx_.write_lock();
    bySvcCtx_.get_container_()[hs.getSvcCtx_p()]=newTransRecord_p;
    bySvcCtx_.write_unlock();
   }

   return newTransRecord_p;
 }else{
   // old transaction, which now attached to some svcCtx
   retval = (*it).second;
   bySvcCtx_.read_lock();
   BySvcCtxIterator it1 = bySvcCtx_.find_(hs.getSvcCtx_p());
   if (it1 != bySvcCtx_.end_()) {
     bySvcCtx_.read_unlock();
     // this SvcCtx currently used by some transaction
     if (memcmp(retval->xid_p,&xid,sizeof(XID))==0) {
       // we in same transaction as previous with this Ctx, 
       // no switch is needed.
     }else{
       // this is not current transaction
       //detach old transaction and attach new, if it is not ended.
       //MutexLocker ml(attachMutex_);
       if (   retval->state==active 
           || retval->state==prepared_commit
           || retval->state==prepared_rollback 
           || retval->state==prepared_readonly) {
         OCITransDetach(hs.getSvcCtx_p(),hs.getError_p(),OCI_DEFAULT);
         sword rc = OCIAttrSet(hs.getSvcCtx_p(),OCI_HTYPE_SVCCTX,
                             retval->ociTrans_p, (ub4)0,
                             OCI_ATTR_TRANS,
                             hs.getError_p());
         if (rc!=OCI_SUCCESS && rc!=OCI_SUCCESS_WITH_INFO) {
           throw OCIException(hs.getError_p());
         }
         OCITrans* ociTrans_p = retval->ociTrans_p; 
         if (timeout==0) timeout = retval->timeout; 
         rc = OCITransStart(hs.getSvcCtx_p(),hs.getError_p(),
                            timeout, OCI_TRANS_RESUME);
         if (rc!=OCI_SUCCESS && rc!=OCI_SUCCESS_WITH_INFO) {
             throw OCIException(hs.getError_p());
         }
         it1->second=it->second;
         bySvcCtx_.write_lock();
         bySvcCtx_.get_container_()[hs.getSvcCtx_p()]=retval;
         bySvcCtx_.write_unlock();
       }else{
         // it's commited or rollbacked or forgetted, do nothing
         
       }
     }
   }else{
     bySvcCtx_.read_unlock();
     // current service context is not associated whith any transaction.
     // but transaction is allocated. 
     //  (possible in 2 cases: 
     //     1. when transaction was allocated but not associated
     //        whith service context (error)
     //            CHECKED: impossible (cleanup writeln)
     //     2. when transaction, associated with this service context
     //        is detached. (we need attach it again, if it is not commited or
     //                      rollbacked)
     //     3. when transaction, associated wiht this service context
     //        is done (commited or rollbacked or forgotted) ?
     if (  retval->state==active 
        || retval->state==prepared_commit 
        || retval->state==prepared_rollback 
        || retval->state==prepared_readonly) {
         
        //MutexLocker ml(attachMutex_);
        OCIAttrSet((dvoid*)hs.getSvcCtx_p(),OCI_HTYPE_SVCCTX,
                retval->ociTrans_p,0,OCI_ATTR_TRANS,hs.getError_p());
        sword rc = OCITransStart(hs.getSvcCtx_p(),hs.getError_p(),
                        (*it).second->timeout,OCI_TRANS_RESUME);
        if (rc!=OCI_SUCCESS && rc!=OCI_SUCCESS_WITH_INFO) {
           throw OCIException(hs.getError_p());
        }
        bySvcCtx_.write_lock();
        bySvcCtx_.get_container_()[hs.getSvcCtx_p()]=(*it).second;
        bySvcCtx_.write_unlock();
     }else{
         // commited or rollbacked or forgetted
     }
   }
   if (incUsage) {
     GradSoft::WriteLocker wl(retval->rwlock);
     retval->usageCount++;
   }
   return retval;
 }// new or old transaction endif
}
 
void   OCITransSet::detach(SessionLevelOCIHandlersSet& hs)
{
 GradSoft::WriteLocker wl(bySvcCtx_.get_rwlock());
 BySvcCtxIterator it = bySvcCtx_.find_(hs.getSvcCtx_p());
 if (it!=bySvcCtx_.end_()) {
    OCITransDetach(hs.getSvcCtx_p(),hs.getError_p(),OCI_DEFAULT);
    bySvcCtx_.erase_(it);
 } 
}

void   OCITransSet::endTransaction(SessionLevelOCIHandlersSet& hs, 
                                   const XID& xid)
{
 XID* xid_p = const_cast<XID*>(&xid);
 ByTransactionIterator it = byTransaction_.find(xid_p);
 if (it==byTransaction_.end_()) {
   // it's impossible
   return;
 }
 TransRecord* tr = it->second;
 tr->rwlock.write_lock();
 --tr->usageCount;
 if (tr->usageCount == 0) {
    byTransaction_.erase(xid_p);
    OCIHandleFree(tr->ociTrans_p,OCI_HTYPE_TRANS);
    tr->rwlock.write_unlock();
    delete tr->xid_p; 
    delete tr; 
 }else{
    tr->rwlock.write_unlock();
 }
 OCISvcCtx* ociSvcCtx_p = hs.getSvcCtx_p();
 bySvcCtx_.erase(ociSvcCtx_p);
}


void OCITransSet::cleanupByTransaction(const XID& xid)
{
 ByTransactionIterator it = byTransaction_.find(const_cast<XID*>(&xid));
 if (it==byTransaction_.end_()) {
   return;
 }
 TransRecord* tr = it->second;
 byTransaction_.erase(const_cast<XID*>(&xid)); 
 cleanupTransRecord(tr);
}

void OCITransSet::cleanupTransRecord(OCITransSet::TransRecord* tr)
{
 OCIHandleFree(tr->ociTrans_p,OCI_HTYPE_TRANS);
 delete tr->xid_p;
 delete tr;
}

#endif
  //CORBA_HAVE_OTS

