/**
 * The test run in SqlTool - like manner, but uses getAsString
 * instead of PrintLib.
 */

#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

/*
 $Id: step7.cpp,v 1.9 2002-12-04 09:29:32 srkr Exp $
*/

#include <iostream>
#include <string>
using namespace std;
using namespace UAKGQuery2;


RecordSet_init* recordSetFactory;




void runQuery(QueryManager_ptr queryManager);

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
           dbManager->create_query_manager("scott","tiger","","Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    runQuery(queryManager);

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

void runQuery(QueryManager_ptr qm)
{
    string queryText("");
    for(;;) {
        cout << "enter sql query or 'q' to quit:";
        cout.flush();
        // getline(cin, queryText);
        // getline did not work as expected (it returned cin's previous input)
        char ch;
        while((ch = cin.get()) != '\n') queryText += ch;
        if (queryText[0] == 'q') break;
        if (queryText[0] == '\0') {
            continue;
        }
        try {
            RecordSet_var recordSet = qm->evaluate_e(queryText.c_str(),"SQL92");
            for (int i = 0; i < recordSet->getNRows(); ++i) {
                for (int j = 0; j < recordSet->getNColumns(); ++j) {
                    CORBA::String_var val = recordSet->getAsStringAt(i,j,"_");
                    cout << val << "|";
                }
                cout << endl;
            }
        }catch( const InvalidFieldType&  ex){
           cerr << "InvalidFieldType" << endl;
        }catch( const InvalidColumn& ex) {
           cerr << "InvalidColumn" << endl;
        }catch( const InvalidRow& ex) {
           cerr << "InvalidRow" << endl;
        }catch(const QueryFlagInvalid& ex){
           cerr << "QueryFlagInvalid:" << ex.why << endl;
        }catch(const QueryProcessingError& ex){
           cerr << "QueryProcessingError" << endl;
           cerr << ex.why << endl;
        }
        queryText = "";
    }
}
