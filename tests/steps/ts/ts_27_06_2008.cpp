#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>

/*
 $Id: ts_27_06_2008.cpp,v 1.4 2008-06-28 14:51:36 rssh Exp $
*/

#include <../3/printLib.h>
//#include <Common/UAKGQueryUtils.h>

#include <fstream>
#include <strstream>
#include <iostream>
using namespace std;
using namespace UAKGQuery2;



RecordSet_init* recordSetFactory;
Numeric_init*   numericFactory;





void run_query(QueryManager_ptr& queryManager, long chunk_size=0);

void print_help();

char login[100];
char password[100];
char db_link[100];
char db_drv[100];
unsigned long chunk_size = 0;

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

    // Set default values
    strcpy(login, "grad");
    strcpy(password, "grad");
    strcpy(db_link, "orcl");
    strcpy(db_drv, "Oracle8");


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
cerr << login << password << db_link << db_drv << endl;
      queryManager =
           dbManager->create_query_manager(login,password,db_link,db_drv,"");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }

    run_query(queryManager.inout(), chunk_size);

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

void run_query(QueryManager_ptr& queryManager, long chunk_size)
{
  CORBA::String_var  selectSql = CORBA::string_dup(
" "
  " select "
  "   id, amount "
  " from  "
  "  onetime_and_casual_services oc "
  " where "
  "  customer_id=2 "
  " order by "
  "  rendering_time desc "
" "
  );
    
  RecordSet_var rs = recordSetFactory->create();
  Query_var qr = queryManager->create_query(selectSql.in(),"");

  CORBA::Boolean more = true;
  try {
        qr->prepare_query(rs);
        qr->execute(rs);
        rs = qr->fetch(0,more);
        qr->destroy();
  } catch (const QueryProcessingError& ex) {
        qr->destroy();
        cerr << "QueryProcessingError:" << ex.why << endl;
        return;
  }


  CORBA::ULong idIndex = rs-> getColumnIndexByName("id");
  CORBA::ULong amountIndex = rs-> getColumnIndexByName("amount");
  int typeId = rs -> getFieldTypeAt(idIndex);
  int typeAmount = rs -> getFieldTypeAt(amountIndex);

  if (typeId == UAKGQuery2::TypeFloat) {
     std::cout << "TypeFloat" << std::endl;
  }

  std::cout << "typeId=" << typeId << std::endl;
  std::cout << "typeAmount=" << typeAmount << std::endl;

  std::cout << "sizeof(CORBA::Float)=" << sizeof(CORBA::Float) << std::endl;
  std::cout << "sizeof(CORBA::ULong)=" << sizeof(CORBA::ULong) << std::endl;

  CORBA::Float flx0 = 1;
  CORBA::ULong ulx0  = *(reinterpret_cast<CORBA::ULong*>(&flx0));
//  CORBA::Long lx0 = ulx0;
//  char ch[4];
//  ch[0] = (lx0 & 0xff);
//  ch[1] = (lx0 >> 8);
//  ch[2] = (lx0 >> 16);
//  ch[3] = (lx0 >> 24);

//  CORBA::Long lx1 = ch[0] + (ch[1]<<8) + (ch[2]<<16) + (ch[3]<<24) ;
//  CORBA::ULong ulx1  = lx1;
//  CORBA::Float flx1 = *reinterpret_cast<CORBA::Float*>(&ulx1);

//  std::cout << "flx0=" << flx0 << std::endl;
//  std::cout << "flx1=" << flx1 << std::endl;

  CORBA::Float flx0_1  = *(reinterpret_cast<CORBA::Float*>(&ulx0));
  std::cout << "flx0_1=" << flx0_1 << std::endl;

  char uch[4];
  uch[0] = (ulx0 & 0xff);
  uch[1] = (ulx0 >> 8);
  uch[2] = (ulx0 >> 16);
  uch[3] = (ulx0 >> 24);

  CORBA::ULong ulx2 = uch[0] + (uch[1]<<8) + (uch[2]<<16) + (uch[3]<<24) ;
  CORBA::Float fx2 = *(reinterpret_cast<CORBA::Float*>(&ulx2));
  std::cout << "fx2=" << fx2 << std::endl;

  printRecordSet(std::cout,rs);

}

void print_help()
{
  cout << "ts_27_06_2008 usage:" << endl;
  cout << "ts_27_06_2008 [-user user] [-password password] [-db_link db]"
          " [-db_drv DB-driver]  " << endl;
}

