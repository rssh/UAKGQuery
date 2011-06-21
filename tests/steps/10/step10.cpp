#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

/*
 $Id: step10.cpp,v 1.9 2003-03-13 13:30:16 rssh Exp $
*/

#include <../3/printLib.h>

#include <fstream>
#include <string>
#include <strstream>
#include <iostream>

using namespace std;
using namespace CORBA;
using namespace UAKGQuery2;

void run_query(QueryManager_ptr queryManager);

void write_wstring(QueryManager_ptr queryManager);
void read_wstring(QueryManager_ptr queryManager);


RecordSet_init* recordSetFactory;
Numeric_init*   numericFactory;


    




void print_help();

 char login[100];
 char password[100];
 char db_link[100];
 char db_drv[100];
 char filename[100];

int main(int argc, char** argv)
{
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    ORB_var orb = ORB_init(argc, argv,CORBA_ORB_INIT_THIRD_ARG);
#else
    ORB_var orb = ORB_init(argc, argv);
#endif
    initUAKGQuery2(orb.in());

    ValueFactory vf=orb->lookup_value_factory(
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

    // Set default values
    strcpy(login, "scott");
    strcpy(password, "tiger");
    strcpy(db_link, "");
    strcpy(db_drv, "Oracle8");
    strcpy(filename, argv[0]);


    // Parsing args
    for (int i=0; i<argc-1; i++)
    {
      if (!strcmp(argv[i], "-user"))
      {
         strcpy(login, argv[i+1]);
      }
      if (!strcmp(argv[i], "-password"))
      {
         strcpy(password, argv[i+1]);
      }
      if (!strcmp(argv[i], "-db_link"))
      {
         strcpy(db_link, argv[i+1]);
      }
      if (!strcmp(argv[i], "-db_drv"))
      {
         strcpy(db_drv, argv[i+1]);
      }
    }

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
           dbManager->create_query_manager(
                                       login,password,db_link,db_drv,"");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    run_query(queryManager);

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

void run_query(QueryManager_ptr queryManager)
{
   print_help();
   write_wstring(queryManager);
   read_wstring(queryManager);
}

void print_help()
{
  cout << "step9 usage:" << endl;
  cout << "step9 [-user user] [-password password] [-db_link Oracle alias]"
          " [-db_drv DB-driver]" << endl;
  cout << "step9 demonstrate how UAKGQuery working with wstring fields" << endl;
}

void write_wstring(QueryManager_ptr queryManager)
{
cerr << "write wstring:" << endl;

  String_var drop_sql = string_dup("drop table wstring_test");
  String_var create_sql = string_dup("create table wstring_test (w nvarchar2(256))");
  String_var insert_sql = string_dup("insert into wstring_test (w) values (:param)");
  String_var sql;


cerr << "Drop table wstring_test" << endl;
  try{
    queryManager->evaluate_e(drop_sql, "");
  }catch(QueryFlagInvalid ex){
    cerr << "QueryProcessingError" << endl;
     /* impossible */
  }catch(QueryProcessingError ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryProcessingError" << endl;
    cerr << ex.why << endl;
  }

  cerr << "Create table wstring_test" << endl;
  try{
    queryManager->evaluate_e(create_sql, "");
  }catch(const QueryFlagInvalid& ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryFlagInvalid" << endl;
    return;
  }catch(const QueryProcessingError& ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryProcessingError" << endl;
    cerr << ex.why << endl;
    return;
  }

  CORBA::WString_var msg =
      CORBA::wstring_dup((const CORBA::WChar*)L"Unicode String");

  RecordSet_var rs=recordSetFactory->create();
  rs->addColumn(":param",TypeWString);
  rs->setFieldSizeAt(0,sizeof(L"Unicode String")); 

  rs->addRow();

  cerr << "Insert wstring to db" << endl;
  try{
    sql = CORBA::string_dup(insert_sql);
    Query_var q = queryManager->create_query(sql, "");

    q->prepare(rs);
    cerr << "prepared" << endl;

    rs->setWStringAt(0,0,msg);

    q->execute(rs);
    q->destroy();
  }catch(const QueryFlagInvalid& ex){
    cerr << "QueryFlagInvalid:"<< ex.why  << endl;
    return;
  }catch(QueryProcessingError ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryProcessingError" << endl;
    cerr << ex.why << endl;
    return;
  }
}

namespace {
    unsigned long wstrlen(const CORBA::WChar* value)
    {
        if (value == 0) return 0;
        const CORBA::WChar* last = value;
        while (*last != 0) ++last;
        return last - value;
    }
}

void read_wstring(QueryManager_ptr queryManager)
{
cerr << "Reading wstring from db" << endl;
String_var sql = string_dup("select w from wstring_test");

RecordSet_var rs = recordSetFactory->create();
Boolean more = true;

  try{
    Query_var q = queryManager->create_query(sql, "");
    q->prepare(rs);
    q->execute(rs);
    rs = q->fetch(1, more);
    if (rs->getNRows()>0)
    {
       CORBA::WString_var w;
	try{
         // "read getWString()" 
         w = rs->getWStringAt(0,0);
         // "read getWString() end" << std::endl;
	} catch (const InvalidFieldType& ) {
	   cerr << "InvalidFieldTypeException" << endl;
	   return;
	} catch (const FieldIsNull& ) {
	   cerr << "FieldIsNull" << endl;
	   return;
	} catch (const CORBA::UserException& ex ) {
	   cerr << "Exception during rs->getWString" << endl;
	   return;
	}
#ifdef HAVE_WCOUT
      std::wstring ws((wchar_t*)w.in());
      std::wcout << L"wstring: " << ws;
      std::cout << std::endl;
#else
      std::cout << "Wstring received\n" << std::endl;
#endif
    } else {
      cerr << "Error: in database no records" << endl;
    }
    q->destroy();
  }catch(QueryFlagInvalid ex){
    cerr << "QueryFlagInvalid" << endl;
    cerr << "more = "<< more << endl;
    return;
  }catch(const QueryProcessingError& ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryProcessingError" << endl;
    cerr << ex.why << endl;
    cerr << "more = "<< more << endl;
    return;
  }
cerr << "Comleted." << endl;  
}
