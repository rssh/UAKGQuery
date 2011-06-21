#ifndef __InterBase_InterBaseQuery_h
#define __InterBase_InterBaseQuery_h

#ifndef __InterBase_IBHandlers_h
#include <InterBase/IBHandlers.h>
#endif

#ifndef __InterBase_InterBaseDBFormatHandler_h
#include <InterBase/InterBaseDBFormatHandler.h>
#endif

#ifndef __RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#ifndef __Common_UAKGQueryBase_h
#include <Common/UAKGQueryBase.h>
#endif

#include <InterBase/InterBaseQueryManager.h>
#include <map>

const bool operator < (const ISC_QUAD& l, const ISC_QUAD& r);

struct IBException {};

class InterBaseQuery : public UAKGQueryBase
{
protected:

  InterBaseQueryManager*  interBaseQueryManager_p_;

  SessionLevelIBHandlersSet* handles_p_;

  isc_tr_handle   trans_handle_;
  isc_stmt_handle stmt_handle_;

  XSQLDA  ISC_FAR *   osqlda_;                 /* output SQLDA */
  XSQLDA  ISC_FAR *   isqlda_;                 /* input SQLDA */

  CORBA::String_var cursor_name_;

  isc_tr_handle* current_transaction_p();

  isc_stmt_handle* current_stmt_p();

  typedef std::map<ISC_QUAD, isc_blob_handle> LobHandles;

  LobHandles lob_handles_;

private:

  bool transaction_started_;

  char error_buffer_[400];

  short flag_;

  long  current_param_record_;

  void bind_next_param();

  UAKGQuery2::RecordSet* buildBindDescription(const XSQLDA& sqlda);

protected:

  UAKGQuery2::FieldType  internalDType2valueType(short dtype, short& dlen, short precision, short scale);

  CORBA::ULong getFieldSize(UAKGQuery2::RecordSet* rs, CORBA::ULong col) const;

public:

  InterBaseQuery(const char* query_text,
                InterBaseQueryManager* interBaseQueryManager_p,
                const char* query_flags
                );

  ~InterBaseQuery();


  void prepare_internal();
  void execute_internal();
  CORBA::Boolean retrieve_internal();

  DBFormatHandler*  getDBFormatHandler_p()  { return &interBaseDBFormatHandler; }


  void check_error(ISC_STATUS* status, const char* operation);

  UAKGQuery2::RecordSet* buildResultBindDescription() { return buildBindDescription(*osqlda_); }


public:


// BLOB
   const void* this_implementation_pointer(size_t& length) {
       length = sizeof(InterBaseQuery*);
       return this;
   }
   const void* open_lob_for_writing(size_t& length);

   const void* open_blob_for_writing(size_t& length) {
       return open_lob_for_writing(length);
   }
   const void* open_clob_for_writing(size_t& length) {
       return open_lob_for_writing(length);
   }
   const void* open_wclob_for_writing(size_t& length) {
       return open_lob_for_writing(length);
   }

   isc_blob_handle open_lob_for_reading(ISC_QUAD& id);
   void close_lob_handle(isc_blob_handle& handle);

   unsigned long get_lob_length(ISC_QUAD& id) {
       return get_lob_length(lob_handles_[id]);
   }
   unsigned long get_lob_length(isc_blob_handle& handle);
   char* read_lob_chunk(ISC_QUAD& id, CORBA::ULong chunkSize
           , unsigned long& actual_lenght, CORBA::Boolean& more
           , UAKGQuery2::FieldType t);
   void write_lob_chunk(ISC_QUAD& id
        , const char* data, unsigned long length);

  void   fillDBError();

  virtual void close();

#ifdef CORBA_HAVE_OTS

 struct NoCurrentTransaction {};

 /**
  * join transaction and returns true if control represent some
  * transaction, otherwise return false.
  **/
 bool  joinTransaction(CosTransactions::Control_ptr);

 // Resource methods.

 CosTransactions::Vote prepare(const XID& xid);

 void  commit(const XID& xid);

 void  rollback(const XID& xid);

 void  forget(const XID& xid);

 void  commit_one_phase(const XID& xid);

 void  endTransaction(const XID& xid);

#endif

private:

  InterBaseQuery();
  InterBaseQuery(const InterBaseQuery&);
  InterBaseQuery& operator=(const InterBaseQuery&);

  void check_type(int db_type, int internal_type);

};

#endif
