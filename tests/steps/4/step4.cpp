#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

#include <stdlib.h>
#include <../3/printLib.h>
 //for atoi

/*
 $Id: step4.cpp,v 1.23 2002-12-04 09:29:31 srkr Exp $
*/

#include <iostream>
using namespace std;
using namespace UAKGQuery2;

CORBA::ORB_ptr myORB;


RecordSet_init* recordSetFactory;


    


void run_query(QueryManager_ptr queryManager);

int main(int argc, char** argv)
{
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, CORBA_ORB_INIT_THIRD_ARG);
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
         dbManager->create_query_manager("intbill","intbill","","Oracle8","");
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
 static const char* query_text = "SELECT id FROM parties WHERE id = :id";

 RecordSet_var recordSet = recordSetFactory->create();

 recordSet->addColumn(":id",TypeLong);

 try {
   Query_var query = queryManager->create_query(query_text,"");
   int quit=0;
   query->prepare_query(recordSet);
   recordSet->addRow();
   while(!quit)
   {
     cout << "enter empno or 'q' to quit:" ;
     cout.flush();
     static char tname[12]; 
     cin.getline(tname,12,'\n');
     if (tname[0]=='\0') continue; 
     if (strcmp(tname,"q")==0) break;
     cout << "1" << endl;
     recordSet->setLongAt(0,0,atoi(tname));
     cout << "2" << endl;
     query->execute(recordSet);
     cout << "3" << endl;
     CORBA::Boolean more=1;
     cout << "4" << endl;
     RecordSet_var rs = query->fetch(0,more);
     printRecordSet(cout,rs);
     cout << "5" << endl;
   }
   query->destroy();
 }catch( const InvalidFieldType&  ex){
   cerr << "InvalidFieldType" << endl;
 }catch(const QueryFlagInvalid& ex){
   cerr << "QueryFlagInvalid:" << ex.why << endl;
 }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
 }
}
