#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <importRC.h>

/*
 $Id: uakgtest1.cpp,v 1.1 2000-06-23 13:13:01 alik Exp $
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
/*
void run_query1(UAKGQueryManager_ptr queryManager)
{
 static const char* query_text = 
	 "select TNAME,TABTYPE from tab where 1=1";

 UAKGCollection_var collection_ = queryManager->create_collection("F1,F2,F1", "UAKGTEST", "1=1", NULL);

 UAKGIterator_var iterator = collection_->create_uakgiterator(true);
 CORBA_Boolean more;
 RecordSeq_var recordSeq = iterator->fetch_records(10, more);
 OctSeq_var octSeq = collection_->retrieve_by_pattern(recordSeq[0]);
// collection_->remove_records_by_pattern(recordSeq[0]);
 printRC(cout,octSeq.in());
 iterator->destroy();
 collection_->destroy();
}
*/
void run_query(UAKGQueryManager_ptr queryManager)
{

  UAKGKeyCollection_var key_collection_ = queryManager->create_key_collection_by_fields(
                 "F1,F2,F1", "UAKGTEST", "1=1", "\0", "F_key");
 cerr << "We are in!"<< endl;
// UAKGKeyCollection_var key_collection_ = queryManager->create_key_collection_by_query(
//       "select F1,F2,F1 from UAKGTEST where 1=1 with key F_key");

 cerr << "We are out!"<< endl;
 String_var key_val = string_dup("first");
 key_val += '\0';
 String_var new_val = string_dup("new");
 new_val += '\0';

 cerr << "We are getting records number!"<< endl;
 ULong num = key_collection_->get_number_of_records();
 cerr << "We've got records number!"<< endl;

 Record_var inpRecord = new Record;
 inpRecord->length(1);
 FieldValueAccess::setString(inpRecord[0], key_val.in());
 Record_var record = key_collection_->retrieve_record_with_key(inpRecord);
 printRecord(cout, record.in());

 RecordDescription_var recordDescription = new RecordDescription;
 OctSeq_var params = new OctSeq;
 key_collection_->evaluate_rc("select @,F2 from UAKGTEST, where @ and 2=2",
	 "", recordDescription, params );
 
 key_collection_->destroy();
}

