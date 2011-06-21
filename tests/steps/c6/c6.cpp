#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importCosQuery.h>
#include <importUAKGQuery.h>
#include <importRC.h>
#include <UAKGQueryInit.h>

/*
 $Id: c6.cpp,v 1.3 2001-12-27 09:54:32 yad Exp $
*/

#include <RCStream.h>

#include <CosQueryFacade/FieldValueAccess.h>
#include <CosQueryFacade/RecordDescriptionAccess.h>

//#include <../3/printLib.h>
#include <Common/UAKGQueryUtils.h>
#include <iostream>
using namespace std;
using namespace UAKGQuery;
using namespace CosQuery;
using namespace CosQueryCollection;
using namespace RC;




void execute_dsql(QueryManager_ptr queryManager_);


int main(int argc, char** argv)
{
  
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv,CORBA_ORB_INIT_THIRD_ARG);
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
    
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
          //dbManager->createQueryManager("gradflow","gradflow","troll.internal.grad.kiev.ua","Oracle8","");
           dbManager->createQueryManager("gradflow","gradflow","troll_db","Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    //*****
    execute_dsql(queryManager);
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

void execute_dsql(QueryManager_ptr queryManager_)
{
  try{
  const char*  ostr;
     Record pattern;
     pattern.length(1);
 
   RecordDescription_var recordDescription= new RecordDescription;
   recordDescription->length(1);  

    RecordSeq_var retRec;
  
    ostr = "select party_id from party_roles2 where role_id=3";  


     retRec = queryManager_->evaluate_records_e(ostr, "");    
     
   
UAKGQueryUtils::printRecordSeq(cerr,retRec);

       ostr = "insert into work_items1 (party_id) values (:party_id)";
       
        recordDescription[0].name = CORBA::string_dup("party_id");
        recordDescription[0].type = TypeLong;
        recordDescription[0].size = sizeof(long);

 ////
  //bug: retRec has type TypeNumeric and recordDescription has TypeLong


        queryManager_->evaluate_records(ostr, "",recordDescription,retRec);    

  }catch(QueryTypeInvalid ex){
    cerr << "QueryTypeInvalid" << endl;
    return;
  }catch(QueryInvalid ex){
    cerr << "QueryInvalid" << endl;
    return;
  }catch(QueryProcessingError ex){
    cerr << "QueryProcessingError" << endl;
    cerr << ex.why << endl;
    return;
  }
   
  

}

