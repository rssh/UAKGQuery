#ifndef __InterBase_InterBaseQueryManager_h
#define __InterBase_InterBaseQueryManager_h

#ifndef __CosQueryPostConfigV2_h
#include <CosQueryPostConfigV2.h>
#endif

#ifndef __InterBase_IBHandlers_h
#include <InterBase/IBHandlers.h>
#endif

#ifndef __Common_UAKGQueryManagerBase_h
#include <Common/UAKGQueryManagerBase.h>
#endif

class InterBaseQuery;

class InterBaseQueryManager: public UAKGQueryManagerBase
{
protected:
 
  SessionLevelIBHandlersPool* hndls_p_;

//  static char* ql_types_strs_[];

public:

/*
  const char* default_ql_type_str_p() const
   { return ql_types_strs_[0]; }

  StringSeq* ql_types_strs()
  {
    return UAKGQueryUtils::szArray2StringSeq(ql_types_strs_);
  }
*/

  InterBaseQueryManager()
   :hndls_p_(NULL)
  {}

  virtual ~InterBaseQueryManager()
  {
    if (is_open() ) close();
  }

  void init(DBConnectionManagers_impl* dbConnectionManagers_p,
            const char* username, 
            const char* password,
            const char* dbLink,
            const char* other);
 
  UAKGQuery2::Query_ptr  create_query(const char* query_text, const char* flags);

  bool is_open() { return hndls_p_ != NULL; }
  
  CORBA::Long get_next_sequence_value(const char* sequence);

  void close();


  PortableServer::ServantBase* getBlobReadingServant();
  PortableServer::ServantBase* getClobReadingServant();
  PortableServer::ServantBase* getWclobReadingServant();
  PortableServer::ServantBase* getBlobWritingServant();
  PortableServer::ServantBase* getClobWritingServant();
  PortableServer::ServantBase* getWclobWritingServant();

protected:

  friend InterBaseQuery;

  SessionLevelIBHandlersPool* getSLHndlsPool_p() { return hndls_p_; }
  
  char* normalize_sql(const char* sql);

};

#endif
