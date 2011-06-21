#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <importRC.h>

/*
 $Id: uakgstep1.cpp,v 1.4 2001-04-16 22:39:00 rssh Exp $
*/

#include <RCStream.h>
#include <UAKGQuery_skel.h>
#include <CosQueryFacade/FieldValueAccess.h>

#include <../3/printLib.h>

ORB_ptr myORB;

void run_query(QueryManager_ptr queryManager);


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

void run_query(QueryManager_ptr queryManager)
{

 UAKGCollection_var collection_ = queryManager->create_collection_by_parts("F1,F2,F1", "UAKGTEST", "1=1", "");
 UAKGIterator_var iterator = collection_->create_uakgiterator();
 Boolean more;
 RecordSeq_var recordSeq = iterator->fetch_records(10, more);
 OctSeq_var octSeq = collection_->retrieve_by_pattern(recordSeq[0]);
 printRC(cout,octSeq.in());
 iterator->destroy();
 collection_->destroy();
}


