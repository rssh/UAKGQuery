#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <importRC.h>

#include <RCStream.h>

#include <../3/printLib.h>

#include <CosQueryServer.h>

#include <sys/timeb.h>

#ifdef _MSC_VER
#include <time.h>
#endif

const ULong maxNRetrieved = 100;
const int maxNRuns = 3;

ORB_ptr myORB;

void run_query(UAKGQueryManager_ptr queryManager);


int main(int argc, char** argv)
{
  
  try {

    ORB_var orb = ORB_init(argc, argv);

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


//    DBConnectionManager_var dbManager = 
//              createDefaultDBConnectionManager(myORB, myPOA);

    UAKGQueryManager_var queryManager;
    try {
      queryManager = 
           dbManager->createQueryManager("intbill","intbill","sun_db","Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    //*****
    // added:
    run_query(queryManager);

    //time to disconnect.
    //
    cout << "disconnecting." << endl;
    queryManager->destroy();

  }catch(SystemException& ex){
#if defined(ORBACUS) && !defined(OB_INTEGER_VERSION)
    OBPrintException(ex);
#else
    cerr << ex << endl;
#endif
    return 1;
  }
  return 0;
}



ULong diff_time(timeb x, timeb y)
{
  double x1 = difftime(x.time,y.time);
  double x2 = x.millitm - y.millitm;
  return (ULong)(x1*1000 + x2);  
}



void run_query(UAKGQueryManager_ptr queryManager)
{
static const char* query_text = "select login, host_ip from seances "
                                " where rownum < 10000"
                   ;

 //evaluate_records:

 RecordDescription_var emptyRecordDescription = new RecordDescription;
 emptyRecordDescription->length(0);

 RecordSeq_var emptyRecordSeq = new RecordSeq;
 emptyRecordSeq->length(0);


 timeb frsTime,sndTime;
 ULong delta[maxNRuns];
 double avg_delta;

 try {
   for(ULong nRetrieved = 0; nRetrieved < maxNRetrieved; ++nRetrieved) {
    avg_delta = 0;
    for(int nRuns = 0; nRuns < maxNRuns; ++nRuns) {
     ftime(&frsTime);
     UAKGQuery_var query = queryManager->create_query(query_text,0);
     query->prepare_query(emptyRecordDescription);
     query->execute_records(emptyRecordSeq);
     Boolean more=TRUE;
     RecordSeq_var recordSeq;
     while(more) {
        recordSeq=query->fetch_records(nRetrieved,more);
      //printRecordSeq(cout,recordSeq.in());
     }
     query->destroy();
     ftime(&sndTime);
     delta[nRuns]= diff_time(sndTime,frsTime);
     //cout << "(i,deltat_t) = " << nRetrieved <<','<< delta[nRuns] << endl;
     avg_delta += delta[nRuns];
    }
    avg_delta /= maxNRuns;
    cout << nRetrieved << ',' << avg_delta << endl; 
   }
 }catch(QueryTypeInvalid ex){
   cerr << "QueryTypeInvalid" << endl;
 }catch(QueryInvalid ex){
   cerr << "QueryInvalid" << endl;
 }catch(QueryProcessingError ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
 }

}

