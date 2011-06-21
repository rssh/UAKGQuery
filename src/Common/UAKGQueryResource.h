#ifndef __Common_UAKGQueryResource_h
#define __Common_UAKGQueryResource_h
#ifdef CORBA_HAVE_OTS

#ifndef __Common_UAKGQueryBase_h
#include <Common/UAKGQueryBase.h>
#endif


class UAKGQueryResource: public POA_CosTransactions::Resource,
                         public PortableServer::RefCountServantBase
{
private:

 UAKGQueryBase * query_p_;
 XID            xid_;
 bool           prepared_;

private:

 class EndGuard
 {
   UAKGQueryResource* qr_p_;   
 public:
   EndGuard(UAKGQueryResource* qr_p):qr_p_(qr_p){}
   ~EndGuard() { qr_p_->end(); }    
 };

 friend EndGuard;

public:

 UAKGQueryResource(UAKGQueryBase* query_p, const XID& xid)
   :query_p_(query_p),
    xid_(xid),
    prepared_(false)
 {
   query_p_->_add_ref();
   query_p_->resource_ = this;
 }

 ~UAKGQueryResource()
 {
   if (query_p_ == NULL) return;
   if (query_p_->resource_ == this) {
     query_p_->resource_ = NULL;
     query_p_->_remove_ref();
   }
   query_p_ = NULL;
 }

 CosTransactions::Vote prepare()
 {
   CosTransactions::Vote vote;
   try {
    vote = query_p_->prepare(xid_);
   }catch(...){
    prepared_ = true;
    vote = CosTransactions::VoteRollback;
    throw;
   }
   prepared_ = true;
   //if (vote==CosTransactions::VoteReadOnly) {
   //  end();
   //}
   return vote;
 }

 void commit()
 {
   if (!prepared_) {
     throw CosTransactions::NotPrepared();
   }
   {
    EndGuard(this);
    query_p_->commit(xid_);
   }
 }

 void rollback()
 {
  EndGuard(this);
  query_p_->rollback(xid_);
 }

 void forget()
 {
  EndGuard(this);
  query_p_->forget(xid_);
 }

 void commit_one_phase()
 { 
  EndGuard(this);
  query_p_->commit_one_phase(xid_);
 }

 void end()
 {
   PortableServer::POA_var poa = _default_POA();
   PortableServer::ObjectId_var oid = poa->servant_to_id(this);
   poa->deactivate_object(oid.in());
 }

private:

 UAKGQueryResource();
 UAKGQueryResource(const UAKGQueryResource&);
 UAKGQueryResource& operator=(const UAKGQueryResource&);

};

#endif
#endif
