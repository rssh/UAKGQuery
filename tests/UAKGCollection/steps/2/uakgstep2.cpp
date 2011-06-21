#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <importRC.h>

/*
 Test for UAKGCollection::evaluate_rc()
 $Id: uakgstep2.cpp,v 1.4 2001-04-16 22:39:00 rssh Exp $
*/

#include <RCStream.h>
#include <UAKGQuery_skel.h>
#include <CosQueryFacade/FieldValueAccess.h>

#ifndef __CosQueryFacade_RecordDescriptionAccess_h
#include <CosQueryFacade/RecordDescriptionAccess.h>
#endif

#ifndef __CosQueryFacade_RecordAccess_h
#include <CosQueryFacade/RecordAccess.h>
#endif

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

 UAKGCollection_var collection_ = queryManager->create_collection_by_parts("F1,F2", "UAKGTEST", "1=1", "");
 OctSeq_var octSeq = 
   collection_->evaluate_rc("select @,F1 from UAKGTEST where @ and 2=2",
      "SQL92", RecordDescriptionAccess::empty(), RecordAccess::emptyOctSeq() );
cout << "main 2" << endl;
 printRC(cout,octSeq.in());
cout << "main 3" << endl;
 collection_->destroy();
cout << "main end" << endl;
}


