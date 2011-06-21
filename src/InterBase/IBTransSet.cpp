/*
 * part of UAKGQueryService
 *  (C) Ruslan Shevchenko, 1999, 2000
 * $Id: IBTransSet.cpp,v 1.5 2002-01-04 13:11:42 srkr Exp $
 */

#ifndef __InterBase_IBTransSet_h
#include <InterBase/IBTransSet.h>
#endif

#ifndef __Common_UAKGQueryUtils_h
#include <Common/UAKGQueryUtils.h>
#endif

#ifndef __InterBase_InterBaseQuery_h
#include <InterBase/InterBaseQuery.h>
#endif

#ifdef CORBA_HAVE_OTS

IBTransSet::StorageIndexedByTransaction  IBTransSet::byTransaction_;
GradSoft::Mutex IBTransSet::attachMutex_;


IBTransSet::TransRecord*  IBTransSet::attach(SessionLevelIBHandlersSet& hs,
                                               const XID&   xid, 
                                               unsigned long timeout,
                                               bool   incUsage)
{
  ByTransactionIterator it = byTransaction_.find(const_cast<XID*>(&xid));
  IBTransSet::TransRecord* retval;
  if (it==byTransaction_.end_()) {
    retval = new IBTransSet::TransRecord();
    retval->xid_p = new XID;
    *(retval->xid_p) = xid;
    retval->usageCount = (incUsage ? 1 : 0);
    retval->timeout = timeout;
    retval->state = active;
    retval->trans_handle = 0L;
    retval->stmt_handle = 0L;

    isc_db_handle db_handle = hs.db_handle();
    ISC_STATUS* status = hs.status();

    if (isc_start_transaction(status,
                              &retval->trans_handle, 
                              1,
                              &db_handle,
                              0,
                              NULL))
    {
        SessionLevelIBHandlersSet::check_error(status, "isc_start_transaction");
    }

    byTransaction_.get_container_()[retval->xid_p]=retval;

  }else{
    retval = (*it).second;
    if (incUsage) {
      GradSoft::WriteLocker wl(retval->rwlock);
      retval->usageCount++;
    }
  }

  return retval;
}


void  IBTransSet::detach(SessionLevelIBHandlersSet& hs)
{
}

void IBTransSet::endTransaction(SessionLevelIBHandlersSet& hs, 
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
    // Close stmt handle

    byTransaction_.erase(xid_p);
    tr->rwlock.write_unlock();
    delete tr->xid_p; 
    delete tr; 
 }else{
    tr->rwlock.write_unlock();
 }
}


void IBTransSet::cleanupByTransaction(const XID& xid)
{
 ByTransactionIterator it = byTransaction_.find(const_cast<XID*>(&xid));
 if (it==byTransaction_.end_()) {
   return;
 }
 TransRecord* tr = it->second;
 byTransaction_.erase(const_cast<XID*>(&xid)); 
 cleanupTransRecord(tr);
}

void IBTransSet::cleanupTransRecord(IBTransSet::TransRecord* tr)
{
 delete tr->xid_p;
 delete tr;
}

#endif
  //CORBA_HAVE_OTS

