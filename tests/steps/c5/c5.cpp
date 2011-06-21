#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importCosQuery.h>
#include <importUAKGQuery.h>
#include <importRC.h>
#include <UAKGQueryInit.h>

#if defined(ORBACUS) && !defined(OB_INTEGER_VERSION)
#include <OB/Util.h>
#else
#if defined(ORBACUS) && (OB_INTEGER_VERSION< 3999951L)
#include <OB/Util.h>
#endif
#endif


/*
 $Id: c5.cpp,v 1.4 2002-02-19 16:24:16 yulp Exp $
*/

#include <RCStream.h>

#include <CosQueryFacade/FieldValueAccess.h>

//#include <../3/printLib.h>
#include <Common/UAKGQueryUtils.h>



void execute_dsql(QueryManager_ptr queryManager);


int main(int argc, char** argv)
{
  
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    ORB_var orb = ORB_init(argc, argv,CORBA_ORB_INIT_THIRD_ARG);
#else
    ORB_var orb = ORB_init(argc, argv);
#endif
    
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
           dbManager->createQueryManager("gradflow","gradflow","troll.internal.grad.kiev.ua","Oracle8","");
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

  }catch(const SystemException& ex){
#ifdef CORBA_SYSTEM_EXCEPTION_IS_STREAMBLE
    cerr << ex << endl;
#else
#if defined(ORBACUS) && !defined(OB_INTEGER_VERSION)
    OBPrintException(ex);
#else
#if defined(ORBACUS) && (OB_INTEGER_VERSION < 3999951L)
    OBPrintException(ex);
#else
    cerr << "System Exception" << endl;
#endif
#endif
#endif
    return 1;
  }
  return 0;
}


void check(string str)
{
cerr<<1<<endl;
}
void check1(const char* str)
{
 cerr<<2<<endl;
}

void execute_dsql(QueryManager_ptr queryManager)
{
  RecordDescription_var recordDescription = new RecordDescription;
  recordDescription->length(1);

       recordDescription[0].name=string_dup(":param");
       recordDescription[0].type=TypeLong;
       recordDescription[0].size=4;    

  RecordSeq_var recordSeq = new RecordSeq;
  recordSeq->length(2);
 
   //for(int i=0;i<1;i++)
    //{
      recordSeq[0].length(1);
      FieldValueAccess::setLong(recordSeq[0][0],22);
      recordSeq[1].length(1);
      FieldValueAccess::setLong(recordSeq[1][0],33);
    //}
  RecordSeq_var rs;
  try{
    queryManager->evaluate_records("insert into pr (id) values(:param)", "", recordDescription, recordSeq);
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
cerr<<(recordDescription[0].name+1)<<endl;
cerr<<recordDescription->length()<<endl;

//check(recordDescription[0].name);
check1(recordDescription[0].name);
 

  //UAKGQueryUtils::printRecordSeq(cerr,rs);
}
