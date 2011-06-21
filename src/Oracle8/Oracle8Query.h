#ifndef __Oracle8_Oracle8Query_h
#define __Oracle8_Oracle8Query_h

#ifndef __Oracle8_OCIHandlers_h
#include <Oracle8/OCIHandlers.h>
#endif

#ifndef __Oracle8_Oracle8DBFormatHandler_h
#include <Oracle8/Oracle8DBFormatHandler.h>
#endif

#ifndef __RecordSet_RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#ifndef __Common_UAKGQueryBase_h
#include <Common/UAKGQueryBase.h>
#endif
                                                         
#ifndef __Oracle8_OCIException_h
#include <Oracle8/OCIException.h>
#endif

#ifndef __GradSoft_deque_INCLUDED
#include <deque>
#ifndef __GradSoft_deque_INCLUDED
#endif
#endif

#ifndef __GradSoft_map_INCLUDED
#include <map>
#ifndef __GradSoft_map_INCLUDED
#endif
#endif


class Oracle8Query : public UAKGQueryBase
{
protected:

  Oracle8QueryManager*  oracle8QueryManager_p_;

  SessionLevelOCIHandlersSet* handles_p_;

  OCIStmt* ociStmt_p_;
  OCIBind** ociParamsBind_pp_;
  OCIDefine** ociResultDefs_pp_;
  bool xa_;
  bool useStrClob_;


//  long current_param_record_;
  ub2       stmtType_;

  void rebind();

  void execute_rebinded();

public:


  Oracle8Query(const char* query_text,
               Oracle8QueryManager* oracle8QueryManager_p,
               const char* query_flags
              );

  ~Oracle8Query();


  void prepare_internal();
  void execute_internal();
  CORBA::Boolean retrieve_internal();

protected:

  friend class Oracle8DBFormatHandler;

  Oracle8DBFormatHandler* dbFormatHandler_;
  DBFormatHandler*  getDBFormatHandler_p() {
      if (dbFormatHandler_ == NULL)
          dbFormatHandler_ = new Oracle8DBFormatHandler(this);
      return dbFormatHandler_;
  }

  OCIEnv*  getOCIEnv_p() 
  { return handles_p_->getEnv_p(); }

  UAKGQuery2::RecordSet* buildResultBindDescription();
  void               setParamsOCIBinds(CORBA::ULong len);
  void               setResultOCIDefs(UAKGQuery2::RecordSet* rd);
  void               defineOCIDefs( UAKGQuery2::RecordSet* rd);
  void               freeOCIBinds_pp(OCIBind**&,DBBindSet*);
  void               freeParamsOCIBinds_pp();
  void               freeResultOCIDefs_pp();

  void   fillDBError();
  void   fillDBError(OCIError*);

  void setWarning(sword status, const char* msg);


  UAKGQuery2::FieldType  internalDType2valueType(ub2 dtype, 
                                                 ub2& dlen, ub2 precision,
                                                 sb1 scale, ub2 charsetid);

  ub2        valueType2externalDType(UAKGQuery2::FieldType vt);

  CORBA::ULong  getFieldSize(UAKGQuery2::RecordSet* rs, CORBA::ULong column);

  CORBA::ULong correctFieldSizeForBind(ub2 externalIdType, 
                                     UAKGQuery2::FieldType fdtype,
                                     CORBA::ULong fdsize);


public:

  void checkError(sword status);

  virtual void close();

   // BLOB

   //void fill_context(OCIEnv*& env, OCISvcCtx*& ct);

  PortableServer::ObjectId*  createTemporaryLobId(ub1 lob_type);

  PortableServer::ObjectId* createClobIdForWriting()
       {  return createTemporaryLobId(OCI_TEMP_CLOB); }

  PortableServer::ObjectId* createBlobIdForWriting()
	  {  return createTemporaryLobId(OCI_TEMP_BLOB); }

  PortableServer::ObjectId* createWclobIdForWriting()
     {  return createTemporaryLobId(OCI_TEMP_CLOB); /*TODO: NCLOB */}

  PortableServer::ObjectId* createLobId(OCILobLocator* lobLocator);

  PortableServer::ObjectId* createBlobIdForReading(void* lobData)
     { return createLobId((OCILobLocator*)lobData); }

  PortableServer::ObjectId* createClobIdForReading(void* lobData)
    { return createLobId((OCILobLocator*)lobData); }

  PortableServer::ObjectId* createWclobIdForReading(void* lobData)
    { return createLobId((OCILobLocator*)lobData); }

  void assignBlob(void** dstLobDataPtr, void* srcLobData) throw (UAKGQuery2::QueryProcessingError);

//   isc_blob_handle open_lob_for_reading(OCILobLocator* lob);

   // void close_temporary_lob(OCILobLocator* lob);

   //unsigned long get_lob_length(OCILobLocator* lob);

   //char* read_lob_chunk(OCILobLocator* lob, CORBA::ULong chunkSize
   //        , unsigned long& actual_lenght, CORBA::Boolean& more, UAKGQuery2::FieldType t);

   //void write_lob_chunk(OCILobLocator* lob
   //     , const char* data, unsigned long length);



#ifdef CORBA_HAVE_OTS

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

public:

  OCIError* ociError_p() { return handles_p_->getError_p(); }

private:

  Oracle8Query();
  Oracle8Query(const Oracle8Query&);
  Oracle8Query& operator=(const Oracle8Query&);

};

#endif
