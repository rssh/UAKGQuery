#ifndef __Common_UAKGQueryBase_h
#define __Common_UAKGQueryBase_h

/**
 * part of Grad-Soft UAKGQuery
 * (C) Grad-Soft Ltd, Kiev, UA
 * http://www.gradsoft.kiev.ua
 **/

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
#endif

#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

#ifndef __Common_DBBind_h
#include <Common/DBBind.h>
#endif

#ifndef __Common_UAKGQueryManagerBase_h
#include <Common/UAKGQueryManagerBase.h>
#endif

#ifndef __RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#ifndef __Common_StringFlagSet_h
#include <Common/StringFlagSet.h>
#endif

#ifdef CORBA_HAVE_OTS
// only ORBacus for now
#include COSTRANSACTIONS_H
#endif


class UAKGQueryResource;
class DBBindSet;

class UAKGQueryBase: virtual public POA_UAKGQuery2::Query,
                     virtual public PortableServer::RefCountServantBase
{
protected:

   UAKGQueryManagerBase* qm_p_;

   char*  szFlags_;
   // synchronizated with flags.
   StringFlagSet flags_;

   char*  queryText_;

   DBBindSet*  resultBindSet_p_;
   DBBindSet*  paramsBindSet_p_;

   bool prepared_;
   bool executed_;
   bool retrieved_;
   bool closed_;

   UAKGQuery2::QueryStatus status_;   

   /**
    * true, if this is select query, (not DDL statement),
    * where we can retrieve rows.
    **/
   bool selectFlg_;

   /**
    * number of retrieved rows.
    */
   CORBA::ULong retrievedNRows_;

   /**
    * number of fetched rows.
    * differences between fetched and retrieved NRows_ are
    * number of records which situated in <code> *resultBindSet_p </code>
    **/
   CORBA::ULong fetchedNRows_;

   /**
    * index of row in resultBindSet.
    */
   CORBA::ULong resultRowIndex_;

   
   /**
    * size of chunk, which is retrieved in one request. 
    * (default is set in flags)
    */
   CORBA::ULong resultChunkSize_;


   bool  error_;
   UAKGQuery2::QueryProcessingError  queryError_;

#ifdef CORBA_HAVE_OTS

   CosTransactions::Current_var     current_;
   CosTransactions::Coordinator_var coordinator_;
   CORBA::String_var               transactionName_;
   XID                             transactionXID_;
   GradSoft::Mutex                 transactionMutex_;

   UAKGQueryResource* resource_;

   friend UAKGQueryResource;

#endif
   void setField(UAKGQuery2::RecordSet* rs, CORBA::ULong row, CORBA::ULong col, const UAKGQuery2::Field& ft);
   void getField(UAKGQuery2::RecordSet* rs, CORBA::ULong row, CORBA::ULong col, UAKGQuery2::Field& fld);

public:

   UAKGQueryBase(const char* queryText, UAKGQueryManagerBase* qm_p,
                 const char* flags);

   virtual ~UAKGQueryBase();

   char*  queryText() throw (CORBA::SystemException)
    { return CORBA::string_dup(queryText_); }
   char*  queryText_p() { return queryText_; }
   const char*  queryText_p() const { return queryText_; }
   const char*  szFlags_p() const { return szFlags_; }

   UAKGQuery2::QueryManager_ptr  query_mgr() throw (CORBA::SystemException);

   UAKGQueryManagerBase*  getUAKGQueryManagerBasePtr()
        { return qm_p_; }

   const char* getDBName_p() const;

  // Lobs

   UAKGQuery2::Blob* create_blob() throw (CORBA::SystemException,
		                          UAKGQuery2::QueryProcessingError);

   UAKGQuery2::Clob* create_clob() throw (CORBA::SystemException,
                                          UAKGQuery2::QueryProcessingError);

   UAKGQuery2::Wclob* create_wclob() throw (CORBA::SystemException,
                                            UAKGQuery2::QueryProcessingError);
   
   //
   // LOB implementation: store database-specific LOB data in ObjectID,
   //  for special POA-s, which provided to us by QueryManager.
   //  then we just generate object reference, all operations will be handled
   //  by default servant of appropriative POA.
   // (in UAKGQLobs - top-level classes for such servants)


   // the goal of next 3 functions:
   //   -- create db-specific LOB data.
   //        OCILobLocator in Oracle,
   //        LOB_HANDLE  in Interbase, and so on
   //   -- save one to object id and return

   virtual PortableServer::ObjectId* createClobIdForWriting() = 0;
   virtual PortableServer::ObjectId* createBlobIdForWriting() = 0;
   virtual PortableServer::ObjectId* createWclobIdForWriting() = 0;

   //
   // transform lobData into ObjectId 
   //
   virtual PortableServer::ObjectId* createBlobIdForReading(void* lobData) = 0;
   virtual PortableServer::ObjectId* createClobIdForReading(void* lobData) = 0;
   virtual PortableServer::ObjectId* createWclobIdForReading(void* lobData) = 0;


   //
   virtual void assignBlob(void** dstLobDataPtr, void* srcLobData) 
                                  throw (UAKGQuery2::QueryProcessingError) = 0;

protected:


   void setBindDescription(DBBindSet*& bindSet_pr, 
                           UAKGQuery2::RecordSet* rs,
                           CORBA::ULong chunkSize)
   {
     if (bindSet_pr!=NULL)  delete bindSet_pr;
     bindSet_pr = new DBBindSet(rs, getLogger(), chunkSize);
     bindSet_pr->setDBFormatHandler_p(this->getDBFormatHandler_p());     
   }

   virtual DBFormatHandler*  getDBFormatHandler_p() = 0;


   //
   // memory managed in accordance of CORBA counting reference discipline
   //   (i. e. client must hold this in RecordSet_var)
   //
   UAKGQuery2::RecordSet* getBindDescription(DBBindSet* dbBindSet_p)
   {
     if (dbBindSet_p == NULL) {
        throwQueryProcessingError(3,"Internal: dbBindSet_p==NULL",false);
     }
     return dbBindSet_p->getDescription();
   }

   void setParamsBindDescription(UAKGQuery2::RecordSet* rs)
   {
     setBindDescription(paramsBindSet_p_,rs,rs->getNRows());
   }

   //
   // memory managed in accordance of CORBA counting reference discipline
   //   (i. e. client must hold this in RecordSet_var)
   //
   UAKGQuery2::RecordSet* getParamsBindDescription()
   {
     return getBindDescription(paramsBindSet_p_);
   }

   UAKGQuery2::RecordSet* getResultBindDescription()
   {
     return getBindDescription(resultBindSet_p_);
   }

   void setResultBindDescription(UAKGQuery2::RecordSet* rd)
   {
     setBindDescription(resultBindSet_p_,rd,resultChunkSize_);
   }

   UAKGQuery2::RecordSet* duplicateResultBindStruct();
public:

   UAKGQuery2::RecordSet*  get_parameters_description()
	   throw(UAKGQuery2::QueryProcessingError,
                 CORBA::SystemException)
   {
      qm_p_->touch();
      return getParamsBindDescription();
   }

   UAKGQuery2::RecordSet*  get_result_description()
   {
      qm_p_->touch();
      return getResultBindDescription();
   }

   //
   //  CORBA counting reference increase in paramsBindSet_p_.
   //  paramsBindSet_p_ is responsible for remove this reference.
   //
   void prepare_local(UAKGQuery2::RecordSet* paramsDescription);

   // the same as prepare, but can be called from remote 
   void prepare_query(UAKGQuery2::RecordSet* paramsDescription)
                               throw(UAKGQuery2::QueryProcessingError,
                                     CORBA::SystemException)		
   { prepare_local(paramsDescription); }
 
   // the same as prepare, but can be called from remote 
   void prepare(UAKGQuery2::RecordSet* paramsDescription)
                               throw(UAKGQuery2::QueryProcessingError,
                                     CORBA::SystemException)		
   { prepare_local(paramsDescription); }

protected:

   void pre_execute(UAKGQuery2::RecordSet* rs);

protected:

   GradSoft::Logger&  getLogger()  { return qm_p_->getLogger(); }
   GradSoft::DebugLogStream&    debugs()     { return getLogger().debugs(); }
   GradSoft::InfoLogStream&     infos()      { return getLogger().infos(); }
   GradSoft::WarningLogStream&  warnings()   { return getLogger().warnings(); }
   GradSoft::ErrorLogStream&    errors()     { return getLogger().errors(); }
   GradSoft::FatalLogStream&    fatals()     { return getLogger().fatals(); }

   bool   logSQL() const  { return qm_p_->logSQL(); }

   friend class DBBindSet;

public:

   //
   // we does not touch recordSet refcounter.
   //
   void  execute(UAKGQuery2::RecordSet* rs)
                     throw(UAKGQuery2::QueryProcessingError,
		           CORBA::SystemException);

   void  execute_e() throw(UAKGQuery2::QueryProcessingError,
		           CORBA::SystemException);

#if defined(MICO) && MICO_BIN_VERSION < 0x020306
   void  execute_inout(UAKGQuery2::RecordSet* rs)
	   throw(UAKGQuery2::QueryProcessingError,
	         CORBA::SystemException);
#else
   void  execute_inout(UAKGQuery2::RecordSet*& rs)
                    throw(UAKGQuery2::QueryProcessingError,
                          CORBA::SystemException);
#endif

   UAKGQuery2::QueryStatus get_status() throw(CORBA::SystemException)
    { return status_  ; }

   UAKGQuery2::RecordSet*  get_all_parameters()
	           throw(UAKGQuery2::QueryProcessingError,
		         CORBA::SystemException);

   UAKGQuery2::RecordSet*  get_parameters(const UAKGQuery2::StringSeq& neededFields)
        throw(UAKGQuery2::QueryProcessingError,
              UAKGQuery2::InvalidParameterName,
              CORBA::SystemException);


   UAKGQuery2::RecordSet*  get_parameter(const char* name)
        throw(UAKGQuery2::QueryProcessingError,
              UAKGQuery2::InvalidParameterName,
              CORBA::SystemException);


   //
   // local call of execute. call this instead execute to be sure, that
   // we does not copy our rs.
   //
   void  execute_local(UAKGQuery2::RecordSet* rs);


   // QueryResult functions:

protected:

   void checkError() { if (error_) throwLastError(); }

   void checkExecuted()
   {
     checkError();
     if (!executed_)
       throwQueryProcessingError(1,"Query yet not executed",false);
   }

   void checkOutParams();

   void checkPrepared()
   {
     checkError();
     if (!prepared_) 
       throwQueryProcessingError(1,"Query yet not prepared",false);
   }

   virtual void fillDBError()=0;

   /**
    * errorCode - error code of your error.
    * msg - message.
    * dbFlg - true, if in underlaying DB we have additional error information,
    *         false  if this is error in CosQuery itself.
    **/
   void throwQueryProcessingError(CORBA::ULong errorCode, const char* msg, bool dbFlg)
   {
     setQueryProcessingError(errorCode,msg,dbFlg);
     throw queryError_;
   }

   /**
    * save error information, and mark error in this query, but
    * do not throw.
    *  Usially, when we don't want to throw error from constructor
    * errorCode - error code of your error.
    * msg - message.
    * dbFlg - true, if in underlaying DB we have additional error information,
    *         false  if this is error in CosQuery itself.
    **/
   void setQueryProcessingError(CORBA::ULong errorCode, 
                                const char* msg, bool dbFlg);

   /**
    * throw last error if it was occured.
    **/
   void throwLastError()
   {
     if (error_)
        throw queryError_;
   }


   bool getDebugLevel() const
   { return qm_p_->getDebugLevel(); }

public:

   CORBA::ULong get_row_count()
           throw(UAKGQuery2::QueryProcessingError,
                 CORBA::SystemException)
   {
     qm_p_->touch();
     checkExecuted();
     return fetchedNRows_;
   } 

   //
   // memory managed in accordance of CORBA counting reference discipline
   //   (i. e. client must hold this in RecordSet_var)
   //
   UAKGQuery2::RecordSet* fetch(CORBA::ULong numberOfRecords, 
                                CORBA::Boolean& b)
           throw(UAKGQuery2::QueryProcessingError,
                 CORBA::SystemException);

   //
   // memory managed in accordance of CORBA counting reference discipline
   //   (i. e. client must hold this in RecordSet_var)
   //
   UAKGQuery2::RecordSet* get_result(CORBA::ULong numberOfRecords,
                                     CORBA::Boolean& b)
                throw(UAKGQuery2::QueryProcessingError,
                      CORBA::SystemException)	  
   { return fetch(numberOfRecords,b); }


   CORBA::ULong skip(CORBA::ULong numberOfRecords, 
                     CORBA::Boolean& more)
                             throw(UAKGQuery2::QueryProcessingError,
		                   CORBA::SystemException);

   /**
    * note: this is remote CORBA method, so it always return
    * pointer to new QueryProcessingError.
    * please, don't use this function inside UAKGQuery implementation.
    **/
   UAKGQuery2::QueryProcessingError*  get_last_error()
   {
     qm_p_->touch();
     return new UAKGQuery2::QueryProcessingError(queryError_);
   }

   void  destroy() throw (CORBA::SystemException);

protected:

   //
   // we does not touch recordSet refcounter.
   //
   void  fetch_records_incremental(CORBA::ULong& nRows, 
                                   CORBA::Boolean& b,
                                   UAKGQuery2::RecordSet* recordSet);


   void  skip_incremental(CORBA::ULong& nRows, 
                          CORBA::Boolean& b, 
                          CORBA::ULong& actualNRows);


   //
   // we does not touch recordSet refcounter.
   //
   void  fetch_retrieved_records(CORBA::ULong& nRows, 
                                 UAKGQuery2::RecordSet* recordSeq);


   void  skip_retrieved(CORBA::ULong& nRows, CORBA::ULong& actualNRows);

   void  fetchBindRowRec(DBBindSet* bindSet, CORBA::ULong row,
                         UAKGQuery2::RecordSet* recordSeq);

   bool  fetchRetrievedRowRec(UAKGQuery2::RecordSet* recordSet);

   void  incFetchIndex();

   virtual void close();

public:

   virtual void prepare_internal(void) = 0;
   virtual void execute_internal(void) = 0;

   // TODO: comment

   virtual CORBA::Boolean retrieve_internal(void) = 0;

   //
   // memory managed in accordance of CORBA counting reference discipline
   //   (i. e. client must hold this in RecordSet_var)
   //
   virtual UAKGQuery2::RecordSet* buildResultBindDescription() = 0;

   // internal
   CORBA::ORB_ptr getORB_() { return qm_p_->getORB_(); }

   PortableServer::POA_ptr getPOA_();
   PortableServer::POA_ptr  _default_POA();


#ifdef CORBA_HAVE_OTS

protected:


public:
 

 bool  joinCurrentTransaction()
 {
   CosTransactions::Control_var control = current_->get_control();
   return joinTransaction(control.in());
 }

 virtual bool  joinTransaction(CosTransactions::Control_ptr)=0;

 virtual void  endTransaction(const XID& xid)=0;

 virtual CosTransactions::Vote prepare(const XID& xid) = 0;

 virtual void  commit(const XID& xid) = 0;

 virtual void  rollback(const XID& xid) = 0;

 virtual void  forget(const XID& xid) = 0;

 virtual void  commit_one_phase(const XID& xid) = 0;

#endif

private:

   UAKGQueryBase();
   UAKGQueryBase(const UAKGQueryBase&);
   UAKGQueryBase& operator=(const UAKGQueryBase&);
};
#endif
