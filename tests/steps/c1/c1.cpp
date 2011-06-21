#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importCosQuery.h>
#include <importUAKGQuery.h>
#include <importRC.h>
#include <UAKGQueryInit.h>

#include <GradSoft/ProgOptions.h>
using namespace GradSoft;

#include <RCStream.h>

#include <../3/printLib.h>
#include <iostream>
using namespace std;
using namespace UAKGQuery;
using namespace CosQuery;


void run_tests(QueryManager_ptr queryManager);


void initProgOptions(ProgOptions& po, int argc, char** argv)
{
  po.put("name","name of database user",true);
  po.put("passwd","password of database user",true);
  po.put("alias","db alias",true);
  po.parse(argc,argv);
}

int main(int argc, char** argv)
{
  
  ProgOptions po("--","",true);
  initProgOptions(po,argc,argv);

  const char* name;
  const char* passwd;
  const char* alias;

  name=po.argument("name");
  passwd=po.argument("passwd");
  alias=po.argument("alias");
  if (alias==NULL) alias = "";
  if (name==NULL || passwd==NULL) {
    cerr << "please, use name and passwd" << endl;
    return 1;
  } 
  
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, CORBA_ORB_INIT_THIRD_ARG);
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
    initUAKGQueryService(orb.in());

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
           dbManager->createQueryManager(name,passwd,alias,"Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    run_tests(queryManager);

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

#define NCOLLECTIONS_1  5
#define COLLECTION_1_NTESTS 2

void run_collection_tests(QueryManager_ptr queryManager,
                          const char* collection_query,
                          const int* expected_exceptions);

void run_tests(QueryManager_ptr queryManager)
{

  // where queries for collection created are stored.
  const char* collection_queries[NCOLLECTIONS_1];

  // expected exceptions:
  //  0 - without exception
  //  1 - excepton "QueryProcessingError"
  //  1 - excepton "QueryInvalid"
  //  2 - excepton "ReadOnlyCollection"
  int expected_exceptions[NCOLLECTIONS_1][COLLECTION_1_NTESTS];
  
  collection_queries[0] = "fejjsdfa;asdf'asdfp"; // incorrect  
  expected_exceptions[0][0]=1;
  expected_exceptions[0][1]=1;

  collection_queries[1] = "SELECT F1,F2 FROM UAKGTEST"; // correct without where
  expected_exceptions[1][0] =  0;
  expected_exceptions[1][1] =  0;

  collection_queries[2] = "SELECT F1,F2 FROM UAKGTEST WHERE F1='qqq'"; 
                                                // correct with where
  expected_exceptions[2][0]=0;
  expected_exceptions[2][1]=0;

  collection_queries[3] = "SELECT F1,F2 FROM UAKGTEST WHERE F1='qqq'"; 
  expected_exceptions[3][0] = 0;
  expected_exceptions[3][1] = 0;

  collection_queries[4] = "SELECT F1,F2,'FROM' FROM UAKGTEST WHERE F1='qqq'"; 
  expected_exceptions[4][0] = 0;
  expected_exceptions[4][1] = 0;
  
  for (int i=0; i<NCOLLECTIONS_1; ++i) {
     run_collection_tests(queryManager,
                          collection_queries[i],expected_exceptions[i]);
  }

}


void run_collection_tests(QueryManager_ptr queryManager,
                          const char* collection_query,
                          const int* expected_exceptions)
{
  cerr << "collection with query:" << collection_query << endl;
  UAKGCollection_var collection;
  try {
    cerr << "create_collection" << endl;
    collection=queryManager->create_collection(collection_query);
  }catch(const CORBA::SystemException& ex){
    cerr << ex << endl;
    return;
  }catch(const QueryProcessingError& ex){
    cerr << "QueryProcessingError:" << ex.why << endl;
    if (expected_exceptions[0]!=1) {
      cerr << "!!!: unexpected" << endl;
    }else{
      cerr << "expected" << endl;
    }
    return;
  } 
  
  try {
    cerr << "collection->selectQueryText()" << endl;
    CORBA::String_var s = collection->selectQueryText();
    cerr << "received:" << s << endl;
  }catch(const CORBA::SystemException& ex){
    cerr << ex << endl;
  }

  try {
    cerr << "collection->destroy()" << endl;
    collection->destroy();
  }catch(const CORBA::SystemException& ex){
    cerr << ex << endl;
  }
}
