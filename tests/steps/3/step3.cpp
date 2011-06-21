#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

#include <iostream>
#include <../3/printLib.h>
using namespace std;
using namespace UAKGQuery2;

void run_query(QueryManager_ptr queryManager);
 

/**
 * added.
 */
RecordSet_init* recordSetFactory;


    

int main(int argc, char** argv)
{
  
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, CORBA_ORB_INIT_THIRD_ARG);
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
    initUAKGQuery2(orb.in());

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
 
    CORBA::ValueFactory vf=orb->lookup_value_factory(
                             "IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0");
    recordSetFactory=RecordSet_init::_downcast(vf);
    if (recordSetFactory==NULL) {
        cerr <<  "Internal error: can't find valuetype factory for RecordSet"
             << endl;
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
           dbManager->create_query_manager("argentic","argentic","stranger","Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    //*****
    // added:
    run_query(queryManager);

    //time to disconnect.
    //
    cout << "disconnecting." << endl;
    queryManager->destroy();

    orb->destroy();
  }catch(const CORBA::SystemException& ex){
    cerr << ex << endl;
    return 1;
  }
  return 0;
}                                      

void run_query(QueryManager_ptr queryManager)
{                                      
   const char* query_text=
       "insert into exchange_rates values('USD',SYSDATE,:rate)";
   
 try {

   UAKGQuery2::RecordSet_var records = recordSetFactory ->  create();

   records ->  addColumn("rate", UAKGQuery2::TypeDouble);

   records ->  addRow(); 
   
   records ->  setDoubleAt(0,0,5.35);
   
   double retdouble = records->getDoubleAt(0,0);

   std::cerr << "just readed double:" << retdouble << std::endl;

   cerr  <<  "Execution..."  <<  endl;
   Query_var query = queryManager ->  create_query(query_text, "");
   cerr  << "Query created" << endl;
   query ->  prepare_query(records);
   cerr  << "prepared" << endl;
   query ->  execute(records);         
   cerr  <<  "Execution ok."  <<  endl;

   CORBA::ULong nRetrieved = 10;
   CORBA::Boolean more = 1;
   cout.precision(20);
   while(more) {
     records = query -> fetch(nRetrieved,more);
     printRecordSet(cout,records);
   }

   cerr  <<  "Parameters are:"  <<  endl;
   records = query -> get_all_parameters();
   printRecordSet(cout,records);

   query -> destroy();

 }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << ex.why << endl;
 }

}

