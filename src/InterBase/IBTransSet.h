#ifndef __InterBase_IBTransSet_h
#define __InterBase_IBTransSet_h

#ifndef __Common_UAKGQueryUtils_h
#include <Common/UAKGQueryUtils.h>
#endif

#ifndef __InterBase_IBHandlers_h
#include <InterBase/IBHandlers.h>
#endif

#ifndef __GradSoft_Threading_h
#include <GradSoft/Threading.h>
#endif

#ifdef CORBA_HAVE_OTS


/**
 * Each element of OCITransSet constists of:
 *     ServiceHandler (Unique to execution thread)
 *     TransactionHandler (Unique to global transaction)
 *     timeout
 *    
 *
 * the main operations are:
 *     attach(ServiceContext,transaction) 
 *
 *     detach(ServiceContex)
 *     
 *     endTransaction(transaction)
 **/
class IBTransSet
{
private:

  struct XIDLess: public binary_function<const XID*, const XID*, bool>
  {
   bool operator()(const XID* x, const XID* y) const
   { 
     return memcmp(x,y,sizeof(XID)) < 0; 
   }
  };


public:

  enum State {
    active, prepared_commit, prepared_readonly, prepared_rollback, 
    commited, commited_with_error,rollbacked, forgotted
  };

  struct TransRecord
  {
   GradSoft::RWLock    rwlock;
   XID*      xid_p;
   int       usageCount;
   unsigned long timeout;
   isc_tr_handle trans_handle;
   isc_stmt_handle stmt_handle;
   State     state;
  };

private:

  typedef GradSoft::threadsafe_uac<map<XID*,TransRecord*,XIDLess> > 
                                                 StorageIndexedByTransaction;

  static GradSoft::Mutex                 attachMutex_;

public:

  static StorageIndexedByTransaction  byTransaction_;

  typedef StorageIndexedByTransaction::iterator ByTransactionIterator;
  typedef StorageIndexedByTransaction::const_iterator 
                                                ByTransactionConstIterator;

public:

  /**
   * attach transaction XID to SvcZCtx from hs, detach it
   * from previous transaction if nesessory.
   *@params
   *  hs        handlers set for transaction.
   *  xid_p     xid of transaction 
   *  timeout   timeout of transaction, if it is 0 and transaction
   *            is known in OCITransSet, than timeot of transaction
   *            is unchanged.
   *  incUsage  increment usage count of this transaction.
   *@returns
   *  TransRecord for this transaction.
   **/
  static TransRecord*  attach(SessionLevelIBHandlersSet& hs,
                       const XID& xid, 
                       unsigned long timeout, bool incUsage);
 
  /**
   * detach transaction from ociSvcCtx_p if one exists.
   **/
   static void  detach(SessionLevelIBHandlersSet& hs);

  /**
   * mark end of transaction
   * (i. e. detach it from attached service handler,
   *  decerement usageCount and delete handle, if
   * nesessory)
   **/
  static void   endTransaction(SessionLevelIBHandlersSet& hs, 
                               const XID& xid_p);

  static GradSoft::Mutex& attachMutex() { return attachMutex_; };

private:

  static void cleanupByTransaction(const XID& xid);

  static void cleanupTransRecord(TransRecord*);

};

#endif
 // CORBA_HAVE_OTS

#endif
