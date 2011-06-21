
#include <InterBase/IBHandlers.h>

#include <InterBase/InterBaseQueryManager.h>

#include <InterBase/InterBaseQuery.h>

#include <algorithm>
using namespace std;

SessionLevelIBHandlersSet::SessionLevelIBHandlersSet(
                            InterBaseQueryManager* queryManager_p,
                            ISC_STATUS* iscStatusVector,
                            char* dpbBuffer,
                            short dpbLength,
                            isc_db_handle dbHandle
                            )
{
  if (iscStatusVector != NULL){
    iscStatusVector_ = iscStatusVector;
    iscStatusVectorAllocated_ = false;
  }else{
    iscStatusVector_ = new ISC_STATUS[20];
    iscStatusVectorAllocated_ = true;
  }

  queryManager_p_ = queryManager_p;
  if (dpbBuffer==NULL) {

    dpb_copy_ = dpbBuffer_ = (char *) malloc(7);
    char* p = dpbBuffer_;
    *p++ = '\1';
    *p++ = isc_dpb_sweep_interval;
    *p++ = '\4';
    long sweep_interval = 16384;
    long l = isc_vax_integer((char ISC_FAR *) &sweep_interval, 4);
    char* d = (char *) &l;
    *p++ = *d++;
    *p++ = *d++;
    *p++ = *d++;
    *p = *d;
    dpbLength_ = 7;

    /* Add user and password to dpb, much easier.  The dpb will be
    **  new memory.
    */
    char* username = queryManager_p_->getUsername_p();
    char* password = queryManager_p_->getPassword_p();
    isc_expand_dpb(&dpbBuffer_, (short ISC_FAR *) &dpbLength_,
                   isc_dpb_user_name, username,
                   isc_dpb_password, password,  NULL);


    //-------------------
    dpbBufferAllocated_ = true;
  }else{
    dpbBuffer_   = dpbBuffer;
    dpbLength_   = dpbLength;
    dpbBufferAllocated_ = false;
  }

  if (dbHandle==0L) {
     dbHandle_ = 0L;
     CORBA::String_var db_link = CORBA::string_dup(queryManager_p->get_dblink());
     if(isc_attach_database(iscStatusVector_,strlen(db_link),db_link,
                      &dbHandle_,dpbLength_,dpbBuffer_)){
        check_error(iscStatusVector_, "isc_attach_database");
     }
     dbHandleAllocated_ = true;

     if (iscStatusVector_[0]=='1' && iscStatusVector_[1]) {
       throw IBSessionBeginException(iscStatusVector_);
       dbHandleAllocated_ = false;
     }
  }else{
     if (dpbBufferAllocated_ ) {
       char* msg = "Error: try to allocate dbBuffer for existing handle";
       throw IBSessionBeginException(msg);
     }
     dbHandle_ = dbHandle;
     dbHandleAllocated_ = false;
  }
} 


SessionLevelIBHandlersSet::~SessionLevelIBHandlersSet()
{
  if (dbHandleAllocated_) {
    isc_detach_database(iscStatusVector_,&dbHandle_);
  }
  if (dpbBufferAllocated_) {
    isc_free(dpbBuffer_);
    free(dpb_copy_);
    dpbBuffer_ = NULL;
  }
  if (iscStatusVectorAllocated_){
    delete[] iscStatusVector_;
    iscStatusVector_  = NULL;
  }

}

SessionLevelIBHandlersSet*  SessionLevelIBHandlersPool::alloc(CORBA::ULong mode)
{
 StorageIteratorType it = storage_.begin();
 SessionLevelIBHandlersSet* pNew;
 if (storage_.size()==0) { 
   pNew = new SessionLevelIBHandlersSet(queryManager_p_);
 } else {
   // TODO:
   //   write some policies which depend from flag. 
   //   for now we implement only polici whith one connection.
   SessionLevelIBHandlersSet* pPrev = (SessionLevelIBHandlersSet*)
                                                      (*storage_.begin());
   pNew = new SessionLevelIBHandlersSet(
                               queryManager_p_,
                               pPrev->status(),
                               pPrev->dpb_buffer(),
                               pPrev->dpb_buffer_length(),
                               pPrev->db_handle()
                                        );
 }
 it = storage_.insert(it,pNew);
 return pNew;
}

void SessionLevelIBHandlersPool::dealloc(SessionLevelIBHandlersSet* hndls_p)
{
 StorageIteratorType it = find(storage_.begin(),storage_.end(),hndls_p);
 if (it==storage_.end()) {
   std::cerr << "Internal Error: deallocated unallocated handler" << std::endl;

 }else{
   storage_.erase(it);
 }
 delete hndls_p;
}

SessionLevelIBHandlersPool::~SessionLevelIBHandlersPool()
{
 for (StorageIteratorType it = storage_.begin(); it!=storage_.end(); ++it)
   delete (*it);
 storage_.erase(storage_.begin(),storage_.end());
}


void SessionLevelIBHandlersSet::check_error(ISC_STATUS* status, const char* operation)
{
  std::cerr << "\n[ PROBLEM ON " << operation << std::endl;
  isc_print_status(status);
  std::cerr << "SQLCODE: " << isc_sqlcode(status) << " ]" << std::endl;
}

