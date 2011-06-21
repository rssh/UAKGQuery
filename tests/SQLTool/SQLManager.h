#ifndef __SQLMANAGER_H_
#define __SQLMANAGER_H_

#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

#ifdef CORBA_HAVE_OTS
#ifdef ORBACUS
#include <OB/OTS.h>
#endif
#endif

#include <../3/printLib.h>

extern CORBA::ORB_ptr myORB;

struct FatalException
{
 int errCode;
 FatalException(int x){
  errCode=x;
 }
};

class SQLManager
{

  UAKGQuery2::QueryManager_var queryManager_;
  CORBA::Boolean connected_;
#ifdef CORBA_HAVE_OTS
  CosTransactions::Current_var transactionsCurrent_;
#endif
  UAKGQuery2::RecordSet_init* recordSetFactory;
  UAKGQuery2::Numeric_init*   numericFactory;

 public:

  SQLManager();

  UAKGQuery2::QueryManager_ptr get_query_manager_p() { return queryManager_.in(); }
  CORBA::Boolean is_connected() { return connected_; }

  void connect( UAKGQuery2::DBConnectionManager_ptr, const char* );
  void disconnect();

  void execute_query(const char* );

#ifdef CORBA_HAVE_OTS
  void  commit();
  void  begin();
  void  rollback();
#endif

 private:

  SQLManager(const SQLManager&); 
  SQLManager& operator=(const SQLManager&); 
};

#endif

