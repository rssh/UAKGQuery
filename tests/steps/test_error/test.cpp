#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importCosQuery.h>
#include <importUAKGQuery.h>
#include <importRC.h>
#include <UAKGQueryInit.h>

#if defined(ORBACUS) && !defined(OB_INTEGER_VERSION)
#include <OB/Util.h>
#else
#if defined(ORBACUS) && (OB_INTEGER_VERSION <= 3999951L)
#include <OB/Util.h>
#endif
#endif

#include <stdlib.h>
 //for atoi

/*
 $Id: test.cpp,v 1.1 2001-11-05 11:10:04 vaa Exp $
*/

#include <RCStream.h>

#include <CosQueryFacade/FieldValueAccess.h>

ORB_ptr myORB;

void run_query(QueryManager_ptr queryManager);


int main(int argc, char** argv)
{
  
  cout << 1 << endl;

  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    ORB_var orb = ORB_init(argc, argv, CORBA_ORB_INIT_THIRD_ARG);
#else
    ORB_var orb = ORB_init(argc, argv);
#endif
    initUAKGQueryService(orb.in());

    cout << 2 << endl;

    Object_var obj;
    try {
       obj = orb->resolve_initial_references("UAKGQueryService");
    }catch(const ORB::InvalidName&){
       cerr << argv[0] << ": can't resolve UAKGQueryService" << endl;
       return 1;
    }

    cout << 3 << endl;
        
    if(is_nil(obj)) {
      cerr << argv[0] << ": UAKGQueryService is a nil object reference" << endl;
      return 1;
    }

    cout << 4 << endl;

    DBConnectionManager_var dbManager = DBConnectionManager::_narrow(obj);
    if (is_nil(dbManager)) {
      cerr << argv[0] << ": can't narrow dbManager to correct type" << endl;
      return 1;
    } 

    cout << 5 << endl;

    QueryManager_var queryManager;
    try {
      queryManager = 
           dbManager->createQueryManager("mr","mr","troll_db","Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    cout << 6 << endl;

    //*****
    // added:
    run_query(queryManager);

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

void run_query(QueryManager_ptr queryManager)
{
 // test query string 
 static const char* query_text = "select OBJECT, Count(*) from GRDPOSITION gp,GRDOBJECTS go"
                                 " where gp.OBJECT = go.id and go.LAYER=1 and go.id=67622"
                                 " group by OBJECT order by OBJECT";
 static const char* query_textB = "select OBJECT, to_char(Count(*)) from GRDPOSITION gp,GRDOBJECTS go"
                                  " where gp.OBJECT = go.id and go.LAYER=1 and go.id=67622"
                                  " group by OBJECT order by OBJECT";
 //evaluate_records:
 try {

   RecordSeq_var rs = queryManager->evaluate_records_e(query_text, "");

   ULong objID = FieldValueAccess::getLong(rs[0][0]);

   Decimal numPnt = FieldValueAccess::getNumeric(rs[0][1]);

   ULong lNumPnt = DecimalAccess::getULong(numPnt);

   cerr<<"ID:"<<objID<<" Count:"<<lNumPnt<<endl;
   // run second request 
   RecordSeq_var rsB = queryManager->evaluate_records_e(query_textB, "");

   ULong objID = FieldValueAccess::getLong(rsB[0][0]);

   String_var sNumPnt = FieldValueAccess::getAsString(rs[0][1],"");

   cerr<<"ID:"<<objID<<" Count:"<<atol(sNumPnt.in())<<endl;

   cerr<<"See Different Result!!!<<endl;

 }catch(QueryTypeInvalid ex){
   cerr << "QueryTypeInvalid" << endl;
 }catch(QueryInvalid ex){
   cerr << "QueryInvalid" << endl;
 }catch(QueryProcessingError ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
 }

}

