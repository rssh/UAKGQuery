#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <importRC.h>

/*
 Test for QueryManager::create_subcollection_by_query
 $Id: uakgstep4.cpp,v 1.4 2000-07-17 13:08:12 alik Exp $
*/

#include <RCStream.h>
#include <UAKGQuery_skel.h>
#include <CosQueryFacade/FieldValueAccess.h>

#include <../3/printLib.h>

ORB_ptr myORB;

void run_query(UAKGQueryManager_ptr queryManager);


int main(int argc, char** argv)
{
  
  try {

    ORB_var orb = ORB_init(argc, argv);
    myORB=orb.in();

    Object_var obj;
    try {
       obj = orb->resolve_initial_references("UAKGQueryService");
    }catch(const ORB::InvalidName&){
       cerr << argv[0] << ": can't resolve UAKGQueryService" << endl;
       return 1;
    }
        
    if(is_nil(obj)) {
      cerr << argv[0] << ": UAKGQueryService is a nil object reference" << endl;
      return 1;
    }


    DBConnectionManager_var dbManager = DBConnectionManager::_narrow(obj);
    if (is_nil(dbManager)) {
      cerr << argv[0] << ": can't narrow dbManager to correct type" << endl;
      return 1;
    } 

    QueryManager_var queryManager;
    try {
      queryManager = 
           dbManager->createQueryManager("scott","tiger","sun_db","Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    // main
    run_query(queryManager);

    //time to disconnect.
    //
    cout << "disconnecting." << endl;
    queryManager->destroy();

  }
  catch(const SystemException& ex)
  {
//    OBPrintException(ex);
    return 1;
  }
  return 0;
}

void run_query( QueryManager_ptr queryManager )
{
cout << "main begin" << endl;
 UAKGCollection_var collection_ = queryManager->create_collection_by_query("select F1,F2 from UAKGTEST where 1=1 order by F2");
cout << "main 1" << endl;
 UAKGIterator_var iterator = collection_->create_uakgiterator();
cout << "main 2" << endl;
 Boolean more;
 RecordSeq_var recordSeq = iterator->fetch_records(10, more);
cout << "main 3" << endl;
 printRecordSeq(cout,recordSeq.in());
cout << "main 4" << endl;
 iterator->destroy();
cout << "main 5" << endl;
 collection_->destroy();
cout << "main end" << endl;
}


