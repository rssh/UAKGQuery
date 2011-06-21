#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importCosQuery.h>
#include <importUAKGQuery.h>
#include <importRC.h>
#include <UAKGQueryInit.h>

#include <RCStream.h>

#include <../3/printLib.h>

#include <CosQueryFacade/RecordDescriptionAccess.h>
#include <CosQueryFacade/RecordAccess.h>
#include <CosQueryFacade/FieldValueAccess.h>

#include <GradSoft/ProgOptions.h>
using namespace GradSoft;
#include <iostream>
using namespace std;
using namespace UAKGQuery;
using namespace CosQuery;
using namespace CosQueryCollection;
using namespace RC;


void create_collection_valid(QueryManager_ptr qm_ptr);

void create_collection_invalid_1(QueryManager_ptr qm_ptr);

void create_collection_invalid_2(QueryManager_ptr qm_ptr);

void collection_get_record_description(QueryManager_ptr qm_ptr);

const int tests_count = 4;

typedef void (*UAKGCollectioTestFunction)(QueryManager_ptr qm_ptr);

UAKGCollectioTestFunction tests[tests_count] = {
          &create_collection_valid,
          &create_collection_invalid_1,
          &create_collection_invalid_2,
          &collection_get_record_description
          };

void run_tests(QueryManager_ptr queryManager);

void printSystemException(ostream& os, const SystemException& ex);

void initProgOptions(ProgOptions& po, int argc, char** argv)
{
  po.put("name","name of database user",true);
  po.put("passwd","password of database user",true);
  po.put("alias","db alias",true);
  po.parse(argc,argv);
}

int main(int argc, char** argv)
{
  
  ProgOptions po;
  initProgOptions(po,argc,argv);

  const char* name;
  const char* passwd;
  const char* alias;

  name=po.argument("name");
  passwd=po.argument("passwd");
  alias=po.argument("alias");
  if (alias==NULL) alias = "";
  if (name==NULL || passwd==NULL) {
    cerr << "please, use name and password" << endl;
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
    cout << "\t BEGIN TESTS." << endl;

    run_tests(queryManager);

    cout << "\t END TESTS." << endl;
    //time to disconnect.
    //
    cout << "disconnecting." << endl;
    queryManager->destroy();
    orb -> destroy();

  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
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
   for (int i =0; i < tests_count; i++)
      tests[i](queryManager);
}


void create_collection_valid(QueryManager_ptr qm_ptr)
{
  cerr << "    Test of valid creation collection by parts." << endl;
  UAKGCollection_var collection;
  try {
    collection = 
         qm_ptr->create_collection_by_parts(" F1, F2 ","UAKGTEST","","");
    cerr << "Collection created." << endl;
  }catch(const SystemException& ex){
    printSystemException(cerr,ex);
    return;
  }catch(const QueryProcessingError& ex){
    cerr << "QueryProcessingError:" << ex.why << endl;
    return;
  } 
  try {
    cerr << "collection->destroy()" << endl;
    collection->destroy();
  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
  }
}


void create_collection_invalid_1(QueryManager_ptr qm_ptr)
{
  cerr << "    Test of invalid creation collection by parts." << endl;
  UAKGCollection_var collection;
  try {
    cout << "Create collection with invalid field name" << endl;
    collection = 
         qm_ptr->create_collection_by_parts("F1,F2_","UAKGTEST","", "");
    cout << "Collection created." << endl;
  }catch(const QueryProcessingError& ex){
    cerr << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
    return;
  } 
  try {
    cerr << "collection->destroy()" << endl;
    collection->destroy();
  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
  }
}

void create_collection_invalid_2(QueryManager_ptr qm_ptr)
{
  UAKGCollection_var collection;
  try {
    cout << "Create collection with invalid table name" << endl;
    collection = 
         qm_ptr->create_collection_by_parts("F1,F2","UAKGTEST_","","");
    cout << "Collection created." << endl;
  }catch(const QueryProcessingError& ex){
    cerr << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
    return;
  } 
  try {
    cerr << "collection->destroy()" << endl;
    collection->destroy();
  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
  }

}

void collection_get_record_description(QueryManager_ptr qm_ptr)
{
  UAKGCollection_var collection;
  try {
    cout << "Test of executing method UAKGCollection::getRecordDescription()" << endl;
    collection = 
         qm_ptr->create_collection_by_parts("F1,F2","UAKGTEST","","");
    cout << "Collection created." << endl;
  }catch(const QueryProcessingError& ex){
    cerr << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
    return;
  } 
  try{
    cout << "collection->getRecordDescription()" << endl;
    RecordDescription_var recDescr = collection->getRecordDescription();
    printRecordDescription(cout, recDescr);
  }catch(const QueryProcessingError& ex){
    cerr << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
    return;
  } 

  try {
    cerr << "collection->destroy()" << endl;
    collection->destroy();
  }catch(const CORBA::SystemException& ex){
    printSystemException(cerr,ex);
  }
}


void printSystemException(ostream& os,const SystemException& ex)
{
    os << ex << endl;
}

