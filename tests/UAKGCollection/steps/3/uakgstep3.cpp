#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <importRC.h>

/*
 Test for UAKGCollection::create_subcollection by query
 $Id: uakgstep3.cpp,v 1.3 2000-07-14 14:43:20 alik Exp $
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

    UAKGQueryManager_var queryManager;
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

void run_query(UAKGQueryManager_ptr queryManager)
{
cout << "main begin" << endl;
 UAKGCollection_var collection_ = queryManager->create_collection_by_fields("tname", "tab", "1=1", "");
cout << "main 1" << endl;
 UAKGCollection_var new_collection_ = collection_->create_subcollection("select F3 from UAKGTEST where F1='aa' and F3='a1' order by F2");
cout << "main 2" << endl;
 UAKGIterator_var iterator = new_collection_->create_uakgiterator();
cout << "main 3" << endl;

 Boolean more;
 RecordSeq_var recordSeq = iterator->fetch_records(10, more);
cout << "main 4" << endl;
 printRecordSeq(cout,recordSeq.in());
cout << "main 5" << endl;

 iterator->destroy();
 new_collection_->destroy();
 collection_->destroy();
cout << "main end" << endl;
}


