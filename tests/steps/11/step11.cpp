#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>
 
/*
 $Id: step11.cpp,v 1.5 2002-12-04 09:29:30 srkr Exp $
*/

#include <../3/printLib.h>
#include <iostream>
using namespace std;
using namespace UAKGQuery2;



RecordSet_init* recordSetFactory;
Numeric_init*   numericFactory;


void run_query(QueryManager_ptr queryManager);

int main(int argc, char** argv)
{
  
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv,CORBA_ORB_INIT_THIRD_ARG);
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
    initUAKGQuery2(orb.in());

    CORBA::ValueFactory vf=orb->lookup_value_factory(
                             "IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0");
    recordSetFactory=RecordSet_init::_downcast(vf);
    if (recordSetFactory==NULL) {
        cerr <<  "Internal error: can't find valuetype factory for RecordSet" 
             << endl;
        return 1;
    }

    vf=orb->lookup_value_factory(
                             "IDL:gradsoft.kiev.ua/UAKGQuery2/Numeric:1.0");
    numericFactory=Numeric_init::_downcast(vf);
    if (numericFactory==NULL) {
        cerr <<  "Internal error: can't find valuetype factory for Numeric" 
             << endl;
        return 1;
    }

    CORBA::Object_var obj;
    try {
       obj = orb->resolve_initial_references("UAKGQueryService");
    }catch(const CORBA::ORB::InvalidName&){
       cerr << argv[0] << ": can't resolve UAKGQueryService" << endl;
       return 1;
    }
        
    if(CORBA::is_nil(obj)) {
      cerr << argv[0] << ": UAKGQueryService is a nil object reference" << endl;
      return 1;
    }

    DBConnectionManager_var dbManager = DBConnectionManager::_narrow(obj);
    if (CORBA::is_nil(dbManager)) {
      cerr << argv[0] << ": can't narrow dbManager to correct type" << endl;
      return 1;
    } 

    QueryManager_var queryManager;
    try {
      queryManager = 
           dbManager->create_query_manager("gradflow","gradflow","troll.internal.grad.kiev.ua","Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    run_query(queryManager);

    //time to disconnect.
    //
    cout << "disconnecting." << endl;
    queryManager->destroy();

  }catch(const CORBA::SystemException& ex){
    cerr << ex << endl;
    return 1;
  }
  return 0;
}

void run_query(QueryManager_ptr queryManager)
{
 try{ 
    const char* sql = "select cn_first_name, id from parties";
    RecordSet_var resultSet = queryManager->evaluate_e(sql, "SQL92");
    printRecordSet(cerr, resultSet);
 }catch( const InvalidFieldType&  ex){
   cerr << "InvalidFieldType" << endl;
 }catch(const QueryFlagInvalid& ex){
   cerr << "QueryFlagInvalid:" << ex.why << endl;
 }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
 }

}
