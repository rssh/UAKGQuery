#ifndef __Oracle8_OCITransSet_h
#define __Oracle8_OCITransSet_h

#ifndef __Common_UAKGQueryUtils_h
#include <Common/UAKGQueryUtils.h>
#endif

#ifndef __Oracle8_OCIHandlers_h
#include <Oracle8/OCIHandlers.h>
#endif

#ifndef __Oracle8_OCIException_h
#include <Oracle8/OCIException.h>
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
class OCITransSet
{
private:

  struct XIDLess: public binary_function<const XID*, const XID*, bool>
  {
   bool operator()(const XID* x, const XID* y) const
   { 
     return memcmp(x,y,sizeof(XID)) < 0; 
   }
  };

  //PORTING detail: MUST be: sizeof(OCISvcCtx*) <= sizeof(unsigned long)
  struct PtrSvcCtxLess: public binary_function<OCISvcCtx*, OCISvcCtx*, bool>
  {
   bool operator()(const OCISvcCtx* x, const OCISvcCtx* y) const
   { return (unsigned long)x < (unsigned long)y; }
  };

  //PORTING detail: MUST be: sizeof(OCITrans*) <= sizeof(unsigned long)
  struct PtrTransLess: public binary_function<OCITrans*, OCITrans*, bool>
  {
   bool operator()(const char* x, const char* y) const
   { return (unsigned long)x < (unsigned long)y; }
  };

public:

  enum State {
    active, prepared_commit, prepared_readonly, prepared_rollback, 
    commited, commited_with_error,rollbacked, forgotted
  };

  struct TransRecord
  {
   GradSoft::RWLock    rwlock;
   OCITrans* ociTrans_p;
   XID*      xid_p;
   int       usageCount;
   unsigned long timeout;
   State     state;
  };

private:

  typedef GradSoft::threadsafe_uac<map<XID*,TransRecord*,XIDLess> > 
                                                 StorageIndexedByTransaction;

  typedef GradSoft::threadsafe_uac<map<OCISvcCtx*,TransRecord*,PtrSvcCtxLess> > 
                                                 StorageIndexedBySvcCtx;

  static StorageIndexedByTransaction  byTransaction_;
  static StorageIndexedBySvcCtx       bySvcCtx_;

  static GradSoft::Mutex                        attachMutex_;

  typedef StorageIndexedByTransaction::iterator ByTransactionIterator;
  typedef StorageIndexedByTransaction::const_iterator 
                                                ByTransactionConstIterator;
  typedef StorageIndexedBySvcCtx::iterator BySvcCtxIterator;

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
  static TransRecord*  attach(SessionLevelOCIHandlersSet& hs,
                       const XID& xid, 
                       unsigned long timeout, bool incUsage);
 
  /**
   * detach transaction from ociSvcCtx_p if one exists.
   **/
   static void  detach(SessionLevelOCIHandlersSet& hs);

  /**
   * mark end of transaction
   * (i. e. detach it from attached service handler,
   *  decerement usageCount and delete handle, if
   * nesessory)
   **/
  static void   endTransaction(SessionLevelOCIHandlersSet& hs, 
                               const XID& xid_p);

  static GradSoft::Mutex& attachMutex() { return attachMutex_; };

private:

  static void cleanupByTransaction(const XID& xid);

  static void cleanupTransRecord(TransRecord*);

};

#endif
 // CORBA_HAVE_OTS

#endif
