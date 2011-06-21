#ifndef __Oracle8_OracleQueryManager_h
#define __Oracle8_OracleQueryManager_h

/*
 * part of UAKGQuery
 * (C) GradSoft Ltd, Kiev, Ukraine
 * http://www.gradsoft.kiev.ua
 */

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __Oracle8_OCIHandlers_h
#include <Oracle8/OCIHandlers.h>
#endif

#ifndef __Common_UAKGQueryManagerBase_h
#include <Common/UAKGQueryManagerBase.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif

#ifndef __RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#ifdef CORBA_HAVE_OTS
#ifdef USE_OB_XA
#include <OB/xa.h>
#endif
#endif


class Oracle8Query;

/**
 * Query manager for Oracle database.
 **/
class Oracle8QueryManager: public UAKGQueryManagerBase
{
protected:
 
  SessionLevelOCIHandlersPool* hndls_p_;
  SessionLevelOCIHandlersSet*  QMWideHandlersSetPtr_;

public:


  Oracle8QueryManager();

  virtual ~Oracle8QueryManager();


  void init(DBConnectionManagers_impl* dbConnectionManagers_p,
            const DBConnectionParams* params_p);

 
  UAKGQueryBase*  createUAKGQuery(const char* query_text, const char* flags)
		  throw(UAKGQuery2::QueryFlagInvalid,
		        CORBA::SystemException);

  bool is_open() { return hndls_p_ != NULL; }
  
  CORBA::Long get_next_sequence_value_internal(const char* sequence)
		  throw (UAKGQuery2::QueryProcessingError,
		         UAKGQuery2::QueryFlagInvalid,
		         CORBA::SystemException);

  void close();

  UAKGLobBase* createBlobReadingServant();
  UAKGLobBase* createClobReadingServant();
  UAKGLobBase* createWclobReadingServant();
  UAKGLobBase* createBlobWritingServant();
  UAKGLobBase* createClobWritingServant();
  UAKGLobBase* createWclobWritingServant();

protected:

  friend class Oracle8Query;
  friend class Oracle8LobBase;

  SessionLevelOCIHandlersPool* getSLHndlsPool_p() { return hndls_p_; }

  OCIEnv*      getOCIEnv_p() { return hndls_p_->getEnv_p(); }
  OCISvcCtx*   getOCISvcCtx_p() { return hndls_p_->getSvcCtx_p(); }
  OCIError*    getOCIError_p() { return hndls_p_->getError_p(); }

  void         checkError(sword oraRC);
  void         fillDBError(UAKGQuery2::QueryProcessingError& queryError, OCIError* ociError_p);
  void         throwQueryProcessingError(CORBA::ULong errorCode,const char* msg,bool dbFlg, OCIError* ociError_p = NULL);


public:

  /**
  *  return sql adapted for custom driver
  **/
  char*   normalize_sql(const char* sql);

private:

  Oracle8QueryManager(const Oracle8QueryManager&);
  Oracle8QueryManager& operator=(const Oracle8QueryManager&);

};

#endif
