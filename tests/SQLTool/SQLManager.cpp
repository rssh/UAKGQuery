#include <SQLManager.h>
#include <iostream>
using namespace std;
using namespace UAKGQuery2;





    


extern CORBA::ORB_ptr myORB;

SQLManager::SQLManager()
{
 connected_ = 0;
#ifdef CORBA_HAVE_OTS
 CORBA::Object_var o ;
 try {
   o = myORB->resolve_initial_references("TransactionCurrent");
 }catch(const CORBA::ORB::InvalidName& ex){
   cerr << "Fatal: TransactionsCurrent is not resolved" << endl;
   throw FatalException(2);
 }
 transactionsCurrent_ = CosTransactions::Current::_narrow(o);
#endif
 CORBA::ValueFactory vf=myORB->lookup_value_factory(
                             "IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0");
 recordSetFactory=RecordSet_init::_downcast(vf);
 if (recordSetFactory==NULL) {
    throw FatalException(3);
 }
 vf=myORB->lookup_value_factory(
                        "IDL:gradsoft.kiev.ua/UAKGQuery2/Numeric:1.0");
 numericFactory=Numeric_init::_downcast(vf);
 if (numericFactory==NULL) {
   throw FatalException(4);
 }

}

void SQLManager::connect(DBConnectionManager_ptr dbManager, 
                         const char* connect_str )
{
 if( connected_ ) disconnect();

 int connect_strlen = strlen(connect_str);
 char *login = new char[connect_strlen];
 char *pwd = new char[connect_strlen];
 char *alias = new char[connect_strlen];
 char *drv = new char[connect_strlen];
 char *flags = new char[connect_strlen];

 int i=0;
 for(; *connect_str == ' '; connect_str++ ) ;
 for(; (*connect_str != '\0') && (*connect_str != ','); connect_str++, i++ )
  login[i] = *connect_str;
 login[i] = '\0';

 i=0; connect_str++;
 for(; *connect_str == ' '; connect_str++ ) ;
 for(; (*connect_str != '\0') && (*connect_str != ','); connect_str++, i++ )
  pwd[i] = *connect_str;
 pwd[i] = '\0';

 i=0; connect_str++;
 for(; *connect_str == ' '; connect_str++ ) ;
 for(; (*connect_str != '\0') && (*connect_str != ','); connect_str++, i++ )
  alias[i] = *connect_str;
 alias[i] = '\0';

 i=0; connect_str++;
 for(; *connect_str == ' '; connect_str++ ) ;
 for(; (*connect_str != '\0') && (*connect_str != ','); connect_str++, i++ )
  drv[i] = *connect_str;
 drv[i] = '\0';
 i=0; connect_str++;
 for(; *connect_str == ' '; connect_str++ ) ;
 for(; (*connect_str != '\0') ; connect_str++, i++ )
  flags[i] = *connect_str;
 flags[i]='\0';

 try 
 {
  cerr << "x:1" << endl;
  queryManager_ = 
        dbManager->create_query_manager(login, pwd, alias, drv, flags);
  cerr << "x:2" << endl;
  cout << "connected: " << login << "," << pwd << "," 
                        << alias << "," << drv << "," << flags << endl;
  connected_ = 1;
 }
 catch(QueryManagerNotFound)
 {
   cerr << "Can't find query manager." << endl;
 }catch(const CORBA::SystemException& ex){
   cerr << ex << endl;
 }

 delete[] login;
 delete[] pwd;
 delete[] alias;
 delete[] drv;
 delete[] flags;
}



void SQLManager::disconnect()
{
 if( !connected_ ) 
 { 
  cout << "There is nothing to disconnect"<< endl; 
  return;
 }
 try
 {
  if( !queryManager_ ) {
    cerr << "IMPOSSIBLE!" << "connected && queryManager_==0" << endl;
    return; 
  }
  queryManager_->destroy();
  connected_ = 0;
 }
 catch(const CORBA::SystemException& ex)
 { 
  cerr << ex << endl;
 }
 cout << "disconnected." << endl;
}

void SQLManager::execute_query(const char* sql_txt )
{
 if( !connected_ )
 {
  cout << "You must first connect to the DB" << endl;
  return;
 }

 cout << "SQL:"<< sql_txt <<endl;

 RecordSet_var rs = recordSetFactory->create();

 try
 {
  cerr << "before queryManager_->create_query" << endl;
  Query_var query=queryManager_->create_query(sql_txt,"");
  cerr << "before prepare" << endl;
  query->prepare_query(rs);
  cerr << "before execute" << endl;
  query->execute(rs);
  cerr << "after execute" << endl;
  CORBA::ULong nRetrieved = 100;
  CORBA::Boolean more=1;
  while( more )
  {
     cout.precision(20);
     {
   	RecordSet_var recordSet=query->fetch(nRetrieved,more);
        printRecordSet(cout,recordSet);
     }
  }
 cerr << "before query::destroy" << endl;
 query->destroy();
 cerr << "after query::destroy" << endl;
 }
 catch( const InvalidFieldType&  ex){
    cerr << "InvalidFieldType" << endl; 
 }catch(const QueryFlagInvalid& ex){
    cerr << "QueryInvalid" << endl; 
 }catch(const QueryProcessingError& ex){
    cerr << "QueryProcessingError:" << endl;
    cerr << ex.why << endl;
 }catch(const CORBA::SystemException& ex){
    cerr << "System Exception" << endl;
    cerr << ex << endl;
 }
}

#ifdef CORBA_HAVE_OTS

void SQLManager::begin()
{
 try {
    transactionsCurrent_->begin();
 }catch(CosTransactions::SubtransactionsUnavailable){
    cerr << "Subtransactions Unavailable" << endl;
 }
}

void SQLManager::rollback()
{
 try {
   transactionsCurrent_->rollback();
 }catch(const CosTransactions::NoTransaction&){
   cerr << "NoTransaction" << endl;
 }
}

void SQLManager::commit()
{
 try {
   transactionsCurrent_->commit(true);
 }catch(const CosTransactions::NoTransaction&){
   cerr << "NoTransaction" << endl;
 }catch(const CosTransactions::HeuristicMixed&){
   cerr << "HeuristicMixed" << endl;
 }catch(const CosTransactions::HeuristicHazard&){
   cerr << "HeuristicHazard" << endl;
 }
}

#endif
