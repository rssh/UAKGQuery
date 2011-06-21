#include <InterBase/InterBaseQuery.h>

#ifdef CORBA_HAVE_OTS

#include <Common/UAKGQueryResource.h>

#include <InterBase/IBTransSet.h>


bool InterBaseQuery::joinTransaction(CosTransactions::Control_ptr control)
{
 if (flags_.get_sz("NoTransaction")!=NULL) {
    return false;
 }
 if (is_nil(control)) {
   // we are not part of transaction, so we can work
   // without transaction, if this is allowed.
   if (flags_.get_sz("TransactionRequired")!=NULL) {
       throw CORBA::TRANSACTION_REQUIRED();
   }else{
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

 bool add_resource = false;
 CORBA::String_var transactionName=foreign->get_transaction_name();
 if (CORBA::is_nil(coordinator_)) {
   coordinator_=CosTransactions::Coordinator::_duplicate(foreign);
   add_resource = true;
 }else{
   if (!strcmp(transactionName.in(),transactionName_.in())) {
      // i. e. same transaction
      transaction_started_ = true;
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
   IBTransSet::attach(*handles_p_,xid,ctx->timeout,add_resource);
 }catch(const IBException& ex){
   fillDBError();
 }

 if (add_resource){
    UAKGQueryResource* r_impl = new UAKGQueryResource(this,xid);
    CORBA::Object_var o = r_impl->_this();
    r_impl->_remove_ref();
    CosTransactions::Resource_var r = CosTransactions::Resource::_narrow(o); 
    coordinator_->register_resource(r);
 }
 transaction_started_ = true;
 return true;

}

void InterBaseQuery::endTransaction(const XID& xid)
{
 transaction_started_ = false;
 try {
   IBTransSet::endTransaction(*handles_p_,xid);
 }catch(const IBException& ex){
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

class InterBaseQueryEndTransactionGuard
{
 InterBaseQuery& owner_;
 const XID& xid_;
public:
 InterBaseQueryEndTransactionGuard(InterBaseQuery& owner, const XID& xid)
  :owner_(owner),xid_(xid) {}
 ~InterBaseQueryEndTransactionGuard()
  { owner_.endTransaction(xid_); }
};

CosTransactions::Vote InterBaseQuery::prepare(const XID& xid)
{
 IBTransSet::TransRecord* tr;
 try {
   tr = IBTransSet::attach(*handles_p_,xid,0,false);
 }catch(const IBException& ex){
   fillDBError();
   return CosTransactions::VoteRollback;
 }

 IBTransSet::State state;
 {
  GradSoft::ReadLocker rl(tr->rwlock);
  state = tr->state;
 }
  
 if (state==IBTransSet::prepared_commit || 
     state==IBTransSet::commited) {
           return CosTransactions::VoteCommit;
 }else if (state==IBTransSet::prepared_readonly) {
           return CosTransactions::VoteReadOnly;
 } else if (state==IBTransSet::rollbacked ||
            state==IBTransSet::prepared_rollback) {
           return CosTransactions::VoteRollback;
 }

 ISC_STATUS* status = handles_p_->status();
 if (isc_prepare_transaction(status, &tr->trans_handle))
 {
    check_error(status, "isc_prepare_transaction");
    tr->state = IBTransSet::prepared_rollback; 
    fillDBError();
    return CosTransactions::VoteRollback;
 }else{
    tr->state = IBTransSet::prepared_commit; 
    return CosTransactions::VoteCommit;
 }

}


void  InterBaseQuery::commit(const XID& xid)
{
  IBTransSet::TransRecord* tr;
  InterBaseQueryEndTransactionGuard endTransactionGuard(*this,xid);
  try {
    tr=IBTransSet::attach(*handles_p_,xid,0,false);
  }catch(const IBException& ex){
    fillDBError();
    throw CosTransactions::HeuristicRollback();
  }

  IBTransSet::State state;
  {
   GradSoft::ReadLocker rl(tr->rwlock);
   state=tr->state;
  }
  if (state==IBTransSet::prepared_commit ||
      state==IBTransSet::prepared_readonly)
  {
    ISC_STATUS *status = handles_p_->status();
    if (tr->trans_handle != 0)
    {
        if (isc_commit_transaction(status, &tr->trans_handle))
        {
            check_error(status, "isc_transaction_commit");
            fillDBError();
            state = IBTransSet::commited_with_error;
        }else{
            state = IBTransSet::commited;
            tr->trans_handle = 0;
        }
    }else{
        state = IBTransSet::commited;
    }
  }else if (state == IBTransSet::active) {
    throw CosTransactions::NotPrepared();
  }else if (state==IBTransSet::rollbacked || 
            state==IBTransSet::prepared_rollback) {
    throw CosTransactions::HeuristicRollback();
  }else if (state==IBTransSet::commited) {
        // do nothing, we already commited
  }else if (state==IBTransSet::commited_with_error) {
        throw CosTransactions::HeuristicHazard();
  }else if (state==IBTransSet::forgotted) {
        throw CosTransactions::HeuristicMixed();
  }
  {
   GradSoft::WriteLocker wl(tr->rwlock);
   tr->state=state; 
  }

}


void  InterBaseQuery::rollback(const XID& xid)
{


  InterBaseQueryEndTransactionGuard endTransactionGuard(*this,xid);
  IBTransSet::TransRecord* tr;
  IBTransSet::State state;
  try {
    tr = IBTransSet::attach(*handles_p_,xid,0,false);
  }catch(const IBException& ex){
    fillDBError();
    throw CosTransactions::HeuristicHazard();
  }

  {
   GradSoft::ReadLocker rl(tr->rwlock);
   state = tr->state;
  }
  if (state==IBTransSet::commited)
    throw CosTransactions::HeuristicCommit();
  if (state==IBTransSet::commited_with_error)
    throw CosTransactions::HeuristicHazard();
  if (state==IBTransSet::rollbacked)
    return;
  if (state==IBTransSet::forgotted)
    return;

  ISC_STATUS *status = handles_p_->status();
  if (isc_rollback_transaction(status, &tr->trans_handle))
  {
    check_error(status, "isc_rollback_transaction");
    fillDBError();
  }

  state=IBTransSet::rollbacked;
  {
    GradSoft::WriteLocker wl(tr->rwlock); 
    tr->state = state;
  }

}

void  InterBaseQuery::forget(const XID& xid)
{
cerr << "InterBaseQuery::forget" << endl;
}

void  InterBaseQuery::commit_one_phase(const XID& xid)
{
cerr << "InterBaseQuery::commit_one_phase" << endl;
}

#endif
