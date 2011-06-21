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


#ifndef __Common_UAKGQueryManagerBase_h
#include <Common/UAKGQueryManagerBase.h>
#endif

#ifndef __Common_DBConnectionManager_h
#include <Common/DBConnectionManager.h>
#endif

#ifndef __RecordSetImpl_h
#include <RecordSet/RecordSetImpl.h>
#endif

#ifndef __GradSoft_sql_INCLUDED
#include <sql.h>
#ifndef  __GradSoft_sql_INCLUDED
#define  __GradSoft_sql_INCLUDED
#endif
#endif                                                         


#ifdef CORBA_HAVE_OTS
#ifdef USE_OB_XA
#include <OB/xa.h>
#endif
#endif


class ODBCQuery;

/**
 * Query manager for Oracle database.
 **/
class ODBCQueryManager: public UAKGQueryManagerBase
{
protected:
 
  SQKHDBC connectionHandle_;

public:


  ODBCQueryManager();

  virtual ~ODBCQueryManager();


  void init(DBConnectionManagers_impl* dbConnectionManagers_p,
            const DBConnectionParams* params_p);

 
  UAKGQueryBase*  createUAKGQuery(const char* query_text, const char* flags)
		  throw(UAKGQuery2::QueryFlagInvalid,
		        CORBA::SystemException);

  bool is_open() { return connectionHandld_ != NULL; }
  
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

  friend class ODBCQuery;
  friend class ODBCLobBase;

 
  void         checkConnectionError();
  void         throwQueryProcessingError(CORBA::ULong errorCode,const char* msg,bool dbFlg);


public:

  /**
  *  return sql adapted for custom driver
  **/
  char*   normalize_sql(const char* sql);

private:

  ODBCQueryManager(const ODBCQueryManager&);
  ODBCQueryManager& operator=(const ODBCQueryManager&);

};

#endif
