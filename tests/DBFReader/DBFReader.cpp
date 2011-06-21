#if !defined(___DBFHEADER_H___)
#include "DBFHeader.h"
#define ___DBFHEADER_H___
#endif

#if !defined(___DBFFORORACLE_H___)
#include "DBFforOracle.h"
#define ___DBFFORORACLE_H___
#endif

#ifndef __UAKGQuery2Init_h
#include <UAKGQuery2Init.h>
#endif

#include <importCORBA.h>
#include <importUAKGQuery.h>

#include <iostream>

#include <RecordSetFactory.h>


using namespace std;
using namespace UAKGQuery2;

#define  STRING_BUFFER   4000

int NecessaryFiedls[] = { 0,1,3,5,6,7,10,12,14,15,16,17,18,19,20,26,28 };  
#define  CURRENT_FIELDS 17 

char* str_beg = "INSERT INTO party_in(code,name,id,del_mark,login,password,fullname,bankname,acc_number,mfo,zkpo,ndsid,taxid,address,phone,agreem,firstday) VALUES(";
char* str_end = ")";

char dbf_file_name[20] = "klientse.dbf";

char            InputString[STRING_BUFFER+1]="";
char            SQLString[STRING_BUFFER+1]="";
DBF_OracleClass dbf_class;
unsigned int    Position = 0;
Query_var       query;

//-------------------------------------
int  HelpMessage();
//-------------------------------------
int main(int argc,char* argv[])
{
   try {

#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
      CORBA::ORB_var orb = CORBA::ORB_init(argc, argv,CORBA_ORB_INIT_THIRD_ARG);
#else
      CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
      initUAKGQuery2(orb.in());

      RecordSetFactory::init(orb.in());

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
          cerr << argv[0] << ": can't narrow dbManager to correct type" << endl;      return 1;
      }

     QueryManager_var queryManager;
     try {
       queryManager =
       dbManager->create_query_manager("scott","tiger","","Oracle8","");
     }catch(QueryManagerNotFound){
           cerr << argv[0] <<": can't find query manager." << endl;
           return 1;
     }
     dbf_class.open(dbf_file_name); 
     strcat(SQLString,str_beg); 
     if ( dbf_class.GetFirst() ) {
        try {
          RecordSet_var recordSet;
          recordSet = 
            dbf_class.CreateTable( NecessaryFiedls, CURRENT_FIELDS, 
				   InputString );
          strcat(SQLString,InputString);
          strcat(SQLString,str_end);
          cout << "Sql string:" << endl;
          cout << SQLString << endl; 
          query=queryManager->create_query(SQLString,"");
          query->prepare_query(recordSet);

          do {
            dbf_class.FillTable( recordSet.inout(), NecessaryFiedls, CURRENT_FIELDS );
      	    query->execute( recordSet );
          } while ( dbf_class.GetNext() );
        }catch( const InvalidFieldType&  ex){
   	   cerr << "InvalidFieldType" << endl;
 	}catch(const QueryFlagInvalid& ex){
   	   cerr << "QueryFlagInvalid:" << ex.why << endl;
 	}catch(const QueryProcessingError& ex){
   	   cerr << "QueryProcessingError" << endl;
	   cerr << ex.why << endl;
 	}
     }
     cout << "query destroy." << endl;
     query->destroy();
     cout << "disconnecting." << endl;
     queryManager->destroy();
     orb->destroy();

  }catch( const CORBA::SystemException& ex){
    cerr << "System Exception" << endl; 
    cerr << ex << endl;
    return 1;
  }
  return 0;
 
}
