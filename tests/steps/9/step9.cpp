#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

/*
 $Id: step9.cpp,v 1.19 2006-01-06 10:52:39 rssh Exp $
*/

//#include <../3/printLib.h>
#include <Common/UAKGQueryUtils.h>

#include <fstream>
#include <strstream>
#include <iostream>

#ifndef WIN32
#include <unistd.h>
#endif

using namespace std;
using namespace UAKGQuery2;



RecordSet_init* recordSetFactory;
Numeric_init*   numericFactory;



void* mico_run_thread(void* pvorb)
{
 ((CORBA::ORB_ptr)pvorb)->run();
 return NULL;
}

void run_query(QueryManager_ptr queryManager, long chunk_size=0);
void write_blob(QueryManager_ptr queryManager, long chunk_size=0);
void read_blob(QueryManager_ptr queryManager, long chunk_size=0);

void print_help();

 char login[100];
 char password[100];
 char db_link[100];
 char db_drv[100];
 char filename[100];
 unsigned long chunk_size = 50000;

int main(int argc, char** argv)
{
  try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv,CORBA_ORB_INIT_THIRD_ARG);
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
    initUAKGQuery2(orb.in());

//    pthread_t micoRunThread;
//    pthread_create(&micoRunThread,NULL,mico_run_thread,orb.in());
//    sleep(1);

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

    // Set default values
    strcpy(login, "argentiferous");
    strcpy(password, "argentiferous");
    strcpy(db_link, "stranger");
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
      if (!strcmp(argv[i], "-filename"))
      {
         strcpy(filename, argv[i+1]);
      }      
      if (!strcmp(argv[i], "-chunksize"))
      {
		char s[16];
         chunk_size = atol(strcpy(s, argv[i+1]));
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
cerr << login << "/" << password << "@" << db_link << " " << db_drv << endl;
      queryManager =
           dbManager->create_query_manager(login,password,db_link,db_drv,"");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    //*****
    // added:
    run_query(queryManager.in(), chunk_size);

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

void run_query(QueryManager_ptr queryManager, long chunk_size)
{
   print_help();
   write_blob(queryManager, chunk_size);
   read_blob(queryManager, chunk_size);
}

void print_help()
{
  cout << "step9 usage:" << endl;
  cout << "step9 [-user user] [-password password] [-db_link Oracle alias | InterBase gdb file]"
          " [-db_drv DB-driver]  [-filename file_to_write_to_blob] [-chunksize size]" << endl;
  cout << "step9 demonstrate how UAKGQuery working with BLOB fields" << endl;
  cout << "It write file to BLOB field then read it from and save as file_from_blob." << endl;
}

void write_blob(QueryManager_ptr queryManager, long chunk_size)
{
cerr << "write blob:" << endl;

  CORBA::String_var drop_sql = CORBA::string_dup("drop table blob_test");
  CORBA::String_var create_sql = CORBA::string_dup("create table blob_test (b blob )");
  CORBA::String_var insert_sql;
  if (!strcmp(db_drv, "Oracle8")){
     insert_sql = CORBA::string_dup("insert into blob_test (b) values (:param)");
  }else{
     insert_sql = CORBA::string_dup("insert into blob_test (b) values (?)");
  }

  CORBA::String_var sql;

cerr << "Drop table blob_test" << endl;
  try{
    sql = CORBA::string_dup(drop_sql);
    queryManager->evaluate_e(sql, "");
  }catch(const InvalidFieldType& ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryTypeInvalid" << endl;
  }catch(const QueryFlagInvalid& ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryFlagInvalid" << endl;
  }catch(const QueryProcessingError& ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryProcessingError" << endl;
    cerr << ex.why << endl;
  }

cerr << "Create table blob_test" << endl;
  try{
    sql = CORBA::string_dup(create_sql);
    queryManager->evaluate_e(sql, "");
  }catch(const InvalidFieldType&  ex){
    cerr << "SQL:" << sql << endl;
    cerr << "InvalidFieldType" << endl;
    return;
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

cerr << "read file to OctSeq" << endl;
  OctSeq_var octSeq = new OctSeq;
  ifstream in(filename, ios::in | ios::binary);
  unsigned long len;
  char ch;
  while (in.get(ch)) {
    len = octSeq->length();
    octSeq->length(octSeq->length()+1);
    octSeq.inout()[len] = ch;
  }
cerr << octSeq->length() << endl;

  RecordSet_var recordSet = recordSetFactory->create();
  if (!strcmp(db_drv, "Oracle8")) {
     recordSet->addColumn(":param",TypeBlob);
  } else
     recordSet->addColumn("?",TypeBlob);
  try {
     recordSet->setFieldSizeAt(0,chunk_size);
 }catch( const InvalidFieldType&  ex){
   cerr << "InvalidFieldType" << endl;
 }catch( const InvalidColumn& ex) {
   cerr << "InvalidColumn" << endl;
 }catch( const InvalidRow& ex) {
   cerr << "InvalidRow" << endl;
 }catch(const QueryFlagInvalid& ex){
   cerr << "QueryFlagInvalid:" << ex.why << endl;
 }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
 }

  recordSet->addRow();

cerr << "Insert blob to db" << endl;
  try{
    sql = CORBA::string_dup(insert_sql);
    Query_var q = queryManager->create_query(sql, "");

    q->prepare(recordSet);
cerr << "prepared" << endl;
    Blob_var bl = q->create_blob();
cerr << "write_blob: write_blob length = " << octSeq->length() <<", chunksize = "<< chunk_size << endl;
	if (chunk_size){
                long n=0;
		for(long i=0; i<octSeq->length(); n++){
			OctSeq_var chunk = new OctSeq;
			for (; i<octSeq->length(); i++){
				long l = chunk->length();
				if (l == chunk_size) break;
				chunk->length(l+1);
				chunk.inout()[l] = octSeq.in()[i];
			}
    		bl->put_chunk(chunk.in());
		}
cerr << " there was "<< n <<" chunks"<< endl;
	}else{
    	bl->put_chunk(octSeq.in());
	}
cerr << "setRaw" << endl;
     recordSet->setBlobAt(0, 0, bl);
cerr << "execute begin" << endl;
    q->execute(recordSet);
cerr << "execute end" << endl;
    q->destroy();
cerr << "execute query destroyed" << endl;
 }catch( const InvalidFieldType&  ex){
   cerr << "InvalidFieldType" << endl;
 }catch( const InvalidColumn& ex) {
   cerr << "InvalidColumn" << endl;
 }catch( const InvalidRow& ex) {
   cerr << "InvalidRow" << endl;
 }catch(const QueryFlagInvalid& ex){
   cerr << "QueryFlagInvalid:" << ex.why << endl;
 }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
 }
cerr << "Completed" << endl;
}

void read_blob(QueryManager_ptr queryManager, long chunk_size)
{
cerr << "READ_BLOB" << endl;
   CORBA::String_var sql = CORBA::string_dup("select b from blob_test");

   RecordSet_var rs = recordSetFactory->create();
    CORBA::Boolean more = true;
  try{
cerr << "read create query" << endl;
    Query_var q = queryManager->create_query(sql, "");
cerr << "read prepare query" << endl;
cerr << "read execute query" << endl;
    q->execute_e();
cerr << "read fetch records" << endl;
        rs = q->fetch(1, more);
cerr << "read fetch records end" << endl;
    if (rs->getNRows()>0)
    {
       Blob_var bl;
	try{
cerr << "read getBlob()" << endl;
         bl = rs->getBlobAt(0,0);
cerr << "read getBlob() end" << endl;
	} catch (const InvalidFieldType& ex) {
	   cerr << "InvalidFieldType" << endl;
	   cerr << "FieldType is " << rs->getAsStringAt(0,0,"Null") << endl;
	   return;
	} catch (const FieldIsNull& ) {
	   cerr << "FieldIsNull" << endl;
	   return;
	}
cerr << "read fetch blob chunk" << endl;
       FILE* f = fopen("file_from_blob", "wb");
       OctSeq_var octSeq;
		long n = 0;
cerr << "read blob chunks to file: chunksize=" << chunk_size << endl;
		for (more=true; more; n++){
                        cerr << "before fetch_chunk" << endl;
			octSeq = bl->fetch_chunk(chunk_size, more);
                        cerr << "after fetch_chunk" << endl;
                        if (octSeq->length()!=0) {
	                 	fwrite(octSeq->get_buffer(), 1, octSeq->length(), f);
                        }
                        cerr << "after fwrite" << endl;
		} fclose(f);
cerr << " more = "<< more <<", there was "<< n <<" chunk(s)"<< endl;
    } else {
      cerr << "Error: in database no records" << endl;
    }
    q->destroy();
  }catch(const InvalidFieldType& ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryTypeInvalid" << endl;
    cerr << "more = "<< more << endl;
    return;
  }catch(const QueryFlagInvalid& ex){
    cerr << "SQL:" << sql << endl;
    cerr << "QueryFlagInvalid" << ex.why << endl;
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

