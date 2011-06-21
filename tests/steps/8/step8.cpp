/**
 * The test run query with rawType parameter. Tables are from CBO/Auth project.
 */

#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

/*
 $Id: step8.cpp,v 1.9 2002-12-04 09:29:33 srkr Exp $
*/

#include <iostream>
#include <string>

using namespace std;
using namespace UAKGQuery2;


RecordSet_init* recordSetFactory;
Numeric_init*   numericFactory;

void runQuery(QueryManager_ptr queryManager);

int main(int argc, char** argv)
{
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv,CORBA_ORB_INIT_THIRD_ARG);
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
    initUAKGQuery2(orb.in());

    CORBA::ValueFactory vf=orb->lookup_value_factory(
                             "IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0");
    recordSetFactory=RecordSet_init::_downcast(vf);
    if (recordSetFactory==NULL) {
        cerr <<  "Internal error: can't find valuetype factory for RecordSet" 
             << endl;
        return 1;
    }

    vf=orb->lookup_value_factory(
                             "IDL:gradsoft.kiev.ua/UAKGQuery2/Numeric:1.0");
    numericFactory=Numeric_init::_downcast(vf);
    if (numericFactory==NULL) {
        cerr <<  "Internal error: can't find valuetype factory for Numeric" 
             << endl;
        return 1;
    }

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
           dbManager->create_query_manager("auth","auth","grad","Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    //*****
    // added:
    runQuery(queryManager);

    //time to disconnect.
    //
    cout << "disconnecting." << endl;
    queryManager->destroy();
    orb->destroy();

  }catch(const CORBA::SystemException& ex){
    cerr << ex << endl;
    return 1;
  }
  return 0;
}

void runQuery(QueryManager_ptr qm)
{
    OctSeq_var passwdHashDecr = new OctSeq();
    passwdHashDecr->length(20);

    UAKGQuery2::RecordSet_var recordSet = recordSetFactory->create();
    recordSet->addColumn(":PASSWD",TypeRaw);
    recordSet->setFieldSizeAt(0,20);

    recordSet->addRow();
    recordSet->setRawAt(0,0,passwdHashDecr);

    /*
     *  user_id NUMBER(10,0), role_id NUMBER(10,0)
     *  can be checked by DESCR AUTH_LOGINS.
     *  Accoding to DataMapping rules should be mapped to Long.
     *  Now returns as Numeric only if Raw type used as bind parameter.
     */
    

    Query_var qr = qm->create_query("select user_id, role_id"
            " from AUTH_LOGINS where login = 'admin' and"
            " (passwd = :PASSWD or passwd is null)","");
    RecordSet_var rs;
    try {
        qr->prepare_query(recordSet);
cerr << "RawTypeBind 1" << endl;
        qr->execute(recordSet);
cerr << "RawTypeBind 2" << endl;
        CORBA::Boolean b;
        rs = qr->fetch(0,b);
        qr->destroy();
    } catch (const QueryProcessingError& ex) {
        qr->destroy();
        cerr << "QueryProcessingError:" << ex.why << endl;
        return;
    }
    if (rs->getNRows() > 0) {
        cout << "Check for field value types (should be Long)" << endl;
	for(CORBA::ULong i=0;i<2;i++) {
	   switch (rs->getFieldTypeAt(i)) {
	   	case TypeNull: 
		    cout << "0 - NULL" << endl;
		    break;
	   	case TypeLong:
		    cout << "0 - Long" << endl;
		    break;
	   	case TypeNumeric:
		    cout << "0 - Numeric" << endl;
		    break;
	   }
	}
    }
    cout << ".passed." << (rs->getNRows() == 0) << endl;
}
