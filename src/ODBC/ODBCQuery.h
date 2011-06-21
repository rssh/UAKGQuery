#ifndef __ODBC_ODBCQuery_h
#define __ODBC_ODBCQuery_h


#ifndef __ODBC_ODBCDBFormatHandler_h
#include <ODBC/ODBCDBFormatHandler.h>
#endif

#ifndef __RecordSet_RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#ifndef __Common_UAKGQueryBase_h
#include <Common/UAKGQueryBase.h>
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

#ifndef __GradSoft_sql_INCLUDED
#include <sql.h>
#ifndef  __GradSoft_sql_INCLUDED
#define  __GradSoft_sql_INCLUDED
#endif
#endif                                                         


class ODBCQuery : public UAKGQueryBase
{
protected:

  ODBCQueryManager*  odbcQueryManager_p_;

  SQLHANDLE  statementHandle_;
  SQLHANDLE  descriptorHandle_;


//  long current_param_record_;
  bool isSelect_;

  void rebind();

  void execute_rebinded();

public:


  ODBCQuery(const char* query_text,
               ODBCQueryManager* odbcQueryManager_p,
               const char* query_flags
              );

  ~ODBCQuery();


  void prepare_internal();
  void execute_internal();
  CORBA::Boolean retrieve_internal();

protected:

  friend class ODBCDBFormatHandler;

  ODBCDBFormatHandler* dbFormatHandler_;
  DBFormatHandler*  getDBFormatHandler_p() {
      if (dbFormatHandler_ == NULL)
          dbFormatHandler_ = new ODBCDBFormatHandler(this);
      return dbFormatHandler_;
  }

  SQLHANDLE  getEnvHandle() 
  { return odbcQueryManager->getEnvHandle(); }

  SQLHANDLE  getConnectionHandle() 
  { return odbcQueryManager->getConnectionHandle(); }



  UAKGQuery2::RecordSet* buildResultBindDescription();

  void               setParamsBinds(CORBA::ULong len);

  void   fillDBError();
  void   fillDBError(OCIError*);

  void setWarning(sword status, const char* msg);



  ub2        valueType2externalDType(UAKGQuery2::FieldType vt);

  CORBA::ULong  getFieldSize(UAKGQuery2::RecordSet* rs, CORBA::ULong column) const;

  CORBA::ULong correctFieldSizeForBind(ub2 externalIdType, 
                                     UAKGQuery2::FieldType fdtype,
                                     CORBA::ULong fdsize);


public:

  void checkError(sword status);

  virtual void close();

   // BLOB

   //void fill_context(OCIEnv*& env, OCISvcCtx*& ct);

  PortableServer::ObjectId*  createTemporaryLobId(ub1 lob_type);

  PortableServer::ObjectId* createClobIdForWriting(int columnNumber)
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

  void assignBlob(void** dstLobDataPtr, void* srcLobData);

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


private:

  ODBCQuery();
  ODBCQuery(const ODBCQuery&);
  ODBCQuery& operator=(const ODBCQuery&);

};

#endif
