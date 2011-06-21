#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importCosQuery.h>
#include <importUAKGQuery.h>
#include <importRC.h>
#include <UAKGQueryInit.h>
/*
 $Id: c4.cpp,v 1.6 2001-12-27 09:54:30 yad Exp $
*/

#include <RCStream.h>

#include <CosQueryFacade/FieldValueAccess.h>

//#include <../3/printLib.h>
#include <Common/UAKGQueryUtils.h>
#include <iostream>
using namespace std;
using namespace UAKGQuery;
using namespace CosQuery;
using namespace CosQueryCollection;
using namespace RC;



void run_query(QueryManager_ptr queryManager);

void print_help();

void execute_sql(QueryManager_ptr queryManager);

void execute_dsql(QueryManager_ptr queryManager);

 char login[100];
 char password[100];
 char db_link[100];
 char db_drv[100];

int main(int argc, char** argv)
{
  
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv,CORBA_ORB_INIT_THIRD_ARG);
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
    //initUAKGQueryService(orb.in());
    
    // Set default values
    strcpy(login, "SYSDBA");
    strcpy(password, "masterkey");
    strcpy(db_link, "/home/srkr/test.gdb");
    strcpy(db_drv, "InterBase");
    cout<<argc<<endl;
    
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
    cerr<<login<<' '<<password<<' '<<db_link<<' '<<db_drv<<"***"<<endl;
      queryManager = 
           dbManager->createQueryManager(login,password,db_link,db_drv,"");
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

  }catch(const CORBA::SystemException& ex){
    cerr << ex << endl;
    return 1;
  }
  return 0;
}

void run_query(QueryManager_ptr queryManager)
{
   char buff[100];
   print_help();
   while (true) {
      cout << "command:";
      // Skip comment lines
      while(true)
      {
        cin.getline(buff, 100, '\n');
	if (buff[0] == '#')
	  continue;
	else
	  break;
      }
      cout << endl;
      if (!strcmp(buff,"sql")){
         execute_sql(queryManager);
         continue;
      }
      if (!strcmp(buff,"dsql")){
         execute_dsql(queryManager);
         continue;
      }
      if (!strcmp(buff,"h")){
         print_help();
         continue;
      }
      if (!strcmp(buff,"q")){
         break;
      }
   }
}

void print_help()
{
  cout << "UAKGQuery SQL tester usage:" << endl;
  cout << "Note: if you use dsql with Oracle all parameters named \":param\" " << endl;
  cout << "	h : print this screen" << endl;
  cout << "	sql : execute sql" << endl;
  cout << "	dsql : execute dynamic sql" << endl;
  cout << "	q : quit" << endl;
}

void execute_sql(QueryManager_ptr queryManager)
{
  char sql[200];
  cout << "enter sql:";
  while(true)
  {
    cin.getline(sql, 200, '\n');
    if (sql[0] == '#')
      continue;
    else
      break;
  }
  cout << endl;
  RecordSeq_var rs;
  try{
    rs = queryManager->evaluate_records_e(sql, "");
  }catch(QueryTypeInvalid ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryTypeInvalid" << endl;
    return;
  }catch(QueryInvalid ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryInvalid" << endl;
    return;
  }catch(QueryProcessingError ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryProcessingError" << endl;
    cerr << ex.why << endl;
    return;
  }
  cerr << "Result for sql: " << sql << endl << endl;
  UAKGQueryUtils::printRecordSeq(cerr,rs);
}

void execute_dsql(QueryManager_ptr queryManager)
{
  char dsql[200];
  cout << "enter dsql:";
  while(true)
  {
    cin.getline(dsql, 200, '\n');
    if (dsql[0] == '#')
      continue;
    else
      break;
  }
  cout << endl;
  cout <<  "enter number of paramenters:";
  char num[100];
  while(true)
  {
    cin.getline(num, 100, '\n');
    if (num[0] == '#')
      continue;
    else
      break;
  }
  cout << endl;
  int params = atoi(num);
  RecordDescription_var recordDescription = new RecordDescription;
  recordDescription->length(params);

  int i=0;
  for (i=0; i<params; i++){
     if (!strcmp(db_drv, "Oracle8"))
       recordDescription[0].name=CORBA::string_dup(":param");
     else
       recordDescription[0].name=CORBA::string_dup("?");
     char param_type[100];
     cout << "param["<<i<<"].type=";
     while(true)
     {
       cin.getline(param_type, 100, '\n');
       if (param_type[0] == '#')
         continue;
	else
	 break;
     }
     cout << endl;
     FieldType type = (FieldType)atoi(param_type);
     recordDescription[i].type=type;
     char param_size[100];
     cout << "param["<<i<<"].size=";
     while(true)
     {
       cin.getline(param_size, 100, '\n');
       if (param_size[0] == '#')
         continue;
	else
	 break;
     }
     cout << endl;
     int psize = atoi(param_size);
     recordDescription[i].size=psize;

  }

  cout << "Enter number of records:";
  while(true)
  {
    cin.getline(num, 100, '\n');
    if (num[0] == '#')
      continue;
    else
      break;
  }
  cout << endl;
  int rec_no = atoi(num);

  RecordSeq_var recordSeq = new RecordSeq;
  recordSeq->length(rec_no);
  for (i=0; i<rec_no; i++)
  {
    cout << "Record[" << i << "]" << endl;
    recordSeq[i].length(params);
    int j=0;
    for (j=0; j<params; j++)
    {
       cout << "Field[" << j << "]=";
       char value[100];
       while(true)
       {
         cin.getline(value, 100, '\n');
	 if (value[0] == '#')
	   continue;
	 else
	   break;
       }
       cout << endl;
       FieldValueAccess::setAsString(recordSeq[i][j], value, recordDescription[j].type);
    }
  }
  UAKGQueryUtils::printRecordSeq(cerr,recordSeq);
  RecordSeq_var rs;
  try{
    rs = queryManager->evaluate_records(dsql, "", recordDescription, recordSeq);
  }catch(QueryTypeInvalid ex){
    cerr << "SQL:" << dsql << endl;
    cerr << "QueryTypeInvalid" << endl;
    return;
  }catch(QueryInvalid ex){
    cerr << "SQL:" << dsql << endl;
    cerr << "QueryInvalid" << endl;
    return;
  }catch(QueryProcessingError ex){
    cerr << "SQL:" << dsql << endl;
    cerr << "QueryProcessingError" << endl;
    cerr << ex.why << endl;
    return;
  }
 
  cerr << "Result for dsql: " << dsql << endl << endl;
  UAKGQueryUtils::printRecordSeq(cerr,rs);
}
