#include <Oracle8/Oracle8Query.h>

#ifdef CORBA_HAVE_OTS

#include <Common/UAKGQueryResource.h>

#include <Oracle8/OCITransSet.h>

bool Oracle8Query::joinTransaction(CosTransactions::Control_ptr control)
{
#ifdef USE_OB_XA
 if (xa_) {
   // all work is in XA intercaeptor
   return true; 
 }
#endif
 if (flags_.get_sz("NoTransaction")!=NULL) {
    return false;
 }
 if (is_nil(control)) {
   // we are not part of transaction, so we can work
   // without transaction, if this is allowed.
   if (flags_.get_sz("TransactionRequired")!=NULL) {
       throw CORBA::TRANSACTION_REQUIRED();
   }else{
        OCITransSet::detach(*handles_p_);
        return false;
   }
 }
 CosTransactions::Coordinator_var foreign;
 try {
   foreign = control->get_coordinator();
 }catch(const CORBA::OBJECT_NOT_EXIST& ){
   // timeout;
   endTransaction(transactionXID_);
   throwQueryProcessingError(1004,"transaction timeout", false);
 }
 //
 // now we know, that foreign transaction exists
 //
 bool add_resource = false;
 CORBA::String_var transactionName=foreign->get_transaction_name();
 if (CORBA::is_nil(coordinator_)) {
   coordinator_=CosTransactions::Coordinator::_duplicate(foreign);
   add_resource = true;
 }else{
   if (!strcmp(transactionName.in(),transactionName_.in())) {
      // i. e. same transaction
      return true;
   }
 }
 transactionMutex_.lock(); // from now we in new transaction
 CosTransactions::PropagationContext_var ctx =  
                                     coordinator_->get_txcontext();
 XID xid;
 UAKGQueryUtils::otid_to_xid(ctx->current.otid,xid);
 transactionName_ = CORBA::string_dup(transactionName.in());
 transactionXID_  = xid;
 try {
   OCITransSet::attach(*handles_p_,xid,ctx->timeout,add_resource);
 }catch(const OCIException& ex){
   fillDBError();
 }
 if (add_resource){
    UAKGQueryResource* r_impl = new UAKGQueryResource(this,xid);
    CORBA::Object_var o = r_impl->_this();
    r_impl->_remove_ref();
    CosTransactions::Resource_var r = CosTransactions::Resource::_narrow(o); 
    coordinator_->register_resource(r);
 }
 return true;
}

void Oracle8Query::endTransaction(const XID& xid)
{
 try {
   OCITransSet::endTransaction(*handles_p_,xid);
 }catch(const OCIException& ex){
   fillDBError();
   // and throw nothing.
 }catch(...){
   transactionName_ = CORBA::string_dup("");
   coordinator_ = CosTransactions::Coordinator::_nil();
   transactionMutex_.unlock();
   throw;
 }
 coordinator_ = CosTransactions::Coordinator::_nil();
 memset(&transactionXID_,0,sizeof(XID));
 transactionName_ = CORBA::string_dup("");
 transactionMutex_.unlock();
}

class Oracle8QueryEndTransactionGuard
{
 Oracle8Query& owner_;
 const XID& xid_;
public:
 Oracle8QueryEndTransactionGuard(Oracle8Query& owner, const XID& xid)
  :owner_(owner),xid_(xid) {}
 ~Oracle8QueryEndTransactionGuard()
  { owner_.endTransaction(xid_); }
};

CosTransactions::Vote Oracle8Query::prepare(const XID& xid)
{
 OCITransSet::TransRecord* tr;
 try {
   tr = OCITransSet::attach(*handles_p_,xid,0,false);
 }catch(const OCIException& ex){
    fillDBError();
    return CosTransactions::VoteRollback;
 }


 OCITransSet::State state;
 {
  GradSoft::ReadLocker rl(tr->rwlock);
  state = tr->state;
 }
  
 if (state==OCITransSet::prepared_commit || 
     state==OCITransSet::commited) {
           return CosTransactions::VoteCommit;
 }else if (state==OCITransSet::prepared_readonly) {
           return CosTransactions::VoteReadOnly;
 } else if (state==OCITransSet::rollbacked ||
            state==OCITransSet::prepared_rollback) {
           return CosTransactions::VoteRollback;
 }

 sword res = OCITransPrepare(handles_p_->getSvcCtx_p(),ociError_p(),OCI_DEFAULT);
 CosTransactions::Vote retval;
  {
   GradSoft::WriteLocker wl(tr->rwlock);
   switch(res){
     case OCI_SUCCESS:
             tr->state = OCITransSet::prepared_commit; 
             retval = CosTransactions::VoteCommit;
             break;
     case OCI_SUCCESS_WITH_INFO:
             tr->state = OCITransSet::prepared_readonly; 
             // for read-only we do nothing with transactions.
             retval = CosTransactions::VoteReadOnly;
             break;
     default:
             tr->state = OCITransSet::prepared_rollback; 
             fillDBError();
             retval = CosTransactions::VoteRollback;
             break;
   }
  }
  //if (retval==CosTransactions::VoteReadOnly) {
  //   endTransaction(xid);
  //}
  return retval; 
}



void  Oracle8Query::commit(const XID& xid)
{
  OCITransSet::TransRecord* tr;
  Oracle8QueryEndTransactionGuard endTransactionGuard(*this,xid);
  try {
    tr=OCITransSet::attach(*handles_p_,xid,0,false);
  }catch(const OCIException& ex){
    fillDBError();
    throw CosTransactions::HeuristicRollback();
  }

  OCITransSet::State state;
  {
   GradSoft::ReadLocker rl(tr->rwlock);
   state=tr->state;
  }
  if (state==OCITransSet::prepared_commit ||
      state==OCITransSet::prepared_readonly){
    OCISvcCtx* ociSvcCtx_p =  handles_p_->getSvcCtx_p();
    sword rc = OCITransCommit(ociSvcCtx_p,ociError_p(),OCI_TRANS_TWOPHASE);
    switch(rc) {
       case OCI_SUCCESS:
       case OCI_SUCCESS_WITH_INFO:
             state = OCITransSet::commited;
             break;
       default:
             fillDBError();
             state = OCITransSet::commited_with_error;
    }
  }else if (state == OCITransSet::active) {
    throw CosTransactions::NotPrepared();
  }else if (state==OCITransSet::rollbacked || 
            state==OCITransSet::prepared_rollback) {
    throw CosTransactions::HeuristicRollback();
  }else if (state==OCITransSet::commited) {
        // do nothing, we already commited
  }else if (state==OCITransSet::commited_with_error) {
        throw CosTransactions::HeuristicHazard();
  }else if (state==OCITransSet::forgotted) {
        throw CosTransactions::HeuristicMixed();
  }
  {
   GradSoft::WriteLocker wl(tr->rwlock);
   tr->state=state; 
  }
}

void  Oracle8Query::rollback(const XID& xid)
{
  Oracle8QueryEndTransactionGuard endTransactionGuard(*this,xid);
  OCITransSet::TransRecord* tr;
  OCITransSet::State state;
  try {
    tr = OCITransSet::attach(*handles_p_,xid,0,false);
  }catch(const OCIException& ex){
    fillDBError();
    throw CosTransactions::HeuristicHazard();
  }
  {
   GradSoft::ReadLocker rl(tr->rwlock);
   state = tr->state;
  }
  if (state==OCITransSet::commited)
    throw CosTransactions::HeuristicCommit();
  if (state==OCITransSet::commited_with_error)
    throw CosTransactions::HeuristicHazard();
  if (state==OCITransSet::rollbacked)
    return;
  if (state==OCITransSet::forgotted)
    return;
  OCISvcCtx* ociSvcCtx_p =  handles_p_->getSvcCtx_p();
  sword res = OCITransRollback(ociSvcCtx_p,ociError_p(),OCI_DEFAULT);
  if (res!=OCI_SUCCESS) {
    fillDBError();
  }
  state=OCITransSet::rollbacked;
  { 
   GradSoft::WriteLocker wl(tr->rwlock); 
   tr->state = state;
  }
}

void  Oracle8Query::forget(const XID& xid)
{
 Oracle8QueryEndTransactionGuard endTransactionGuard(*this,xid);
 OCITransSet::TransRecord* tr;
 OCITransSet::State state;
 try {
    tr=OCITransSet::attach(*handles_p_,xid,0,false);
 }catch(const OCIException& ex){
    fillDBError();
 }
 OCISvcCtx* ociSvcCtx_p =  handles_p_->getSvcCtx_p();
 {
  GradSoft::ReadLocker rl(tr->rwlock);
  state=tr->state;
 }
 OCITransForget(ociSvcCtx_p,ociError_p(),OCI_DEFAULT);
 state=OCITransSet::forgotted;
 {
  GradSoft::WriteLocker wl(tr->rwlock);
  tr->state=OCITransSet::forgotted; 
 }
}

void  Oracle8Query::commit_one_phase(const XID& xid)
{
 OCITransSet::TransRecord* tr;
 Oracle8QueryEndTransactionGuard endTransactionGuard(*this,xid);
 try {
   tr=OCITransSet::attach(*handles_p_,xid,0,false);
 }catch(const OCIException& ex){
   fillDBError();
   throw CosTransactions::HeuristicHazard();
 }
 OCITransSet::State state;
 {
  GradSoft::ReadLocker rl(tr->rwlock);
  state = tr->state; 
 }
 if (state==OCITransSet::commited) {
   // do nothing;
 } 
 if (state==OCITransSet::commited_with_error ||
     state==OCITransSet::rollbacked||
     state==OCITransSet::forgotted)
   throw CosTransactions::HeuristicHazard();
 OCISvcCtx* ociSvcCtx_p =  handles_p_->getSvcCtx_p();
 sword res = OCITransCommit(ociSvcCtx_p,ociError_p(),OCI_DEFAULT);
 switch(res){
     case OCI_SUCCESS:
     case OCI_SUCCESS_WITH_INFO:
             state=OCITransSet::commited;
             return;
     default:
             fillDBError();
             throw CosTransactions::HeuristicHazard();
 }
 {
  GradSoft::WriteLocker wl(tr->rwlock);
  tr->state = state;
 } 
}

#endif
