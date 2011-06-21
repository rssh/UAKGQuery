#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importCosQuery.h>
#include <importUAKGQuery.h>
#include <importRC.h>
#include <UAKGQueryInit.h>

#include <RCStream.h>

#include <iomanip>
#include <iostream>


#include <CosQueryFacade/RecordDescriptionAccess.h>
#include <CosQueryFacade/RecordAccess.h>
#include <CosQueryFacade/FieldValueAccess.h>
#include <CosQueryFacade/DecimalAccess.h>

#include <GradSoft/ProgOptions.h>
#include <GradSoft/Logger.h>
using namespace GradSoft;

#include <../3/printLib.h>

using namespace std;
using namespace UAKGQuery;
using namespace CosQuery;
using namespace CosQueryCollection;
using namespace RC;

#define LOG_DEBUG_ENABLE true
#define LOG_INFO_ENABLE true
#define LOG_ERROR_ENABLE true


UAKGCollection_var collection = NULL;
//UAKGKeyCollection_var keycollection = NULL;
UAKGIterator_var iter = NULL;
String_var select_part;
String_var from_part;
String_var where_part;
String_var order_part;
String_var key_part;
bool created = false;
bool created_iterator = false;
bool created_as_key = false;

void create_collection(QueryManager_ptr qm_ptr);
void create_key_collection(QueryManager_ptr qm_ptr);
void create_collection_by_sql(QueryManager_ptr qm_ptr);
void create_key_collection_by_sql(QueryManager_ptr qm_ptr);
void destroy_collection();
void destroy_key_collection();
void create_iterator();
void create_iterator_by_pattern();
void destroy_iterator();
void create_subcollection();
void create_subcollection_by_pattern();
void collection_get_record_description();
void collection_set_read_only();
void collection_get_read_only();
void collection_get_number_of_records();
void collection_retrieve_by_filter();
void collection_retrieve_by_pattern();
void collection_sort();
void collection_get_sql();
void collection_remove_all_records();
void collection_remove_record();
void collection_add_record();
void collection_add_rc();
void collection_update_by_filter();
void collection_update_by_pattern();
void collection_remove_records_by_filter();
void collection_remove_records_by_pattern();
void iterator_fetch_rc();
void iterator_fetch_records();
void iterator_skip();
void get_key_description();
void retrieve_record_with_key();
void update_record_with_key();
void remove_record_with_key();
void retrieve_records_with_keys();
void update_records_with_keys();
void remove_records_with_keys();
Record* get_record(RecordDescription_var descr);
OctSeq* records_to_rc(const RecordSeq& recseq, const RecordDescription& descr);

void printSystemException(ostream& os,const SystemException& ex);

#ifndef CORBA_SYSTEM_EXCEPTION_IS_STREAMBLE
ostream& operator<<(ostream& os, const SystemException& ex)
{
 printSystemException(os,ex);
 return os;
}
#endif

Logger logger("file.log",false);

void error_callback(const char* msg)
{
  cerr << msg;
}

void info_callback(const char* msg)
{
  cout << msg;
}

typedef void (*UAKGCollectioTestFunction)(QueryManager_ptr qm_ptr);

void run_tests(QueryManager_ptr queryManager);



void initProgOptions(ProgOptions& po, int argc, char** argv)
{
  po.put("name","name of database user",true);
  po.put("passwd","password of database user",true);
  po.put("alias","db alias",true);
  po.parse(argc,argv);
}

int main(int argc, char** argv)
{
  
  logger.setCallback(Error,error_callback);
  logger.setCallback(Fatal,error_callback);
  logger.setCallback(Info,info_callback);

  ProgOptions po("--","",true);
  initProgOptions(po,argc,argv);

  const char* name;
  const char* passwd;
  const char* alias;

  name=po.argument("name");
  passwd=po.argument("passwd");
  alias=po.argument("alias");
  if (alias==NULL) alias = "";
  if (name==NULL || passwd==NULL) {
    logger.errors() << "please, use name and password" << endl;
    return 1;
  } 

  try {
cerr << 0 << endl;
#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, CORBA_ORB_INIT_THIRD_ARG);
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
#endif
    initUAKGQueryService(orb.in());

cerr << 1 << endl;
    CORBA::Object_var obj;
    try {
       obj = orb->resolve_initial_references("UAKGQueryService");
    }catch(const CORBA::ORB::InvalidName&){
       logger.errors() << argv[0] << ": can't resolve UAKGQueryService" << endl;
       return 1;
    }
        
cerr << 2 << endl;
    if(CORBA::is_nil(obj)) {
      logger.errors() << argv[0] << ": UAKGQueryService is a nil object reference" << endl;
      return 1;
    }


cerr << 3 << endl;
    DBConnectionManager_var dbManager = DBConnectionManager::_narrow(obj);
    if (CORBA::is_nil(dbManager)) {
      logger.errors() << argv[0] << ": can't narrow dbManager to correct type" << endl;
      return 1;
    } 

cerr << 4 << endl;
    QueryManager_var queryManager;
    try {
      queryManager = 
           dbManager->createQueryManager(name,passwd,alias,"Oracle8","");
    }catch(QueryManagerNotFound){
      logger.errors() << argv[0] <<": can't find query manager." << endl;
      return 1;
    }
cerr << 5 << endl;
    logger.infos() << "\t BEGIN TESTS." << endl;
//    try{
      run_tests(queryManager);
//    }catch(...){
//      cerr << "Fatal error." << endl;
//    }
    destroy_collection();

    logger.infos() << "\t END TESTS." << endl;
    //time to disconnect.
    //
    logger.infos() << "disconnecting." << endl;
    queryManager->destroy();
    orb -> destroy();

  }catch(const CORBA::SystemException& ex){
    logger.errors()<< ex << endl;
    return 1;
  }
  return 0;
}

void printHelp(){
    cout << "\tTESTS OF UAKGCollections." << endl;
    cout << "   Choose command to execute from list below : " << endl;
    cout << "create : Create collection.";
    if (created){
      cout << "  (Now you have instance of created collection)" << endl;
    } else {
      cout << "  (Not created yet)" << endl;
    }
    cout << "create_by_sql : Create collection by sql." << endl;
    cout << "create_key : Create KeyCollection." << endl;
    cout << "create_key_by_sql : Create KeyCollection by sql." << endl;
    cout << "subcollection : Create subcollection." << endl;
    cout << "subcollection_by_pattern : Create subcollection by pattern." << endl;
    cout << "destroy : Destroy collection." << endl;
    cout << "iterator : Create iterator." << endl;
    cout << "iterator_by_patern : Create iterator by pattern." << endl;
    cout << "destroy_iterator : Destroy iterator." << endl;
    cout << "description : Get record description." << endl;
    cout << "set_readonly : Set read only." << endl;
    cout << "get_readonly : Get read only." << endl;
    cout << "count : Get number of records." << endl;
    cout << "filter : Retrieve by filter." << endl;
    cout << "pattern : Retrieve by pattern." << endl;
    cout << "add : Add record." << endl;
    cout << "add_rc : Add record in RC format." << endl;
    cout << "update_by_filter : Update record by filter." << endl;
    cout << "update_by_pattern : Update record by pattern." << endl;
    cout << "delete : Delete record." << endl;
    cout << "delete_all : Delete all records." << endl;
    cout << "delete_by_filter : Delete records by filter." << endl;
    cout << "delete_by_pattern : Delete records by pattern." << endl;
    cout << "iterator_fetch_rc : Fetch records in RC format." << endl;
    cout << "iterator_fetch_records : Fetch records." << endl;
    cout << "iterator_skip : Skip records." << endl;
    cout << "keydescription : Get key description." << endl;
    cout << "get_record_with_key : Retreive record with key." << endl;
    cout << "update_record_with_key : Update record by key." << endl;
    cout << "remove_record_with_key : Remove record by key." << endl;
    cout << "get_records_with_keys : Retriece records with keys." << endl;
    cout << "update_records_with_keys : Update records with keys." << endl;
    cout << "remove_records_with_keys : Remove records with keys." << endl;
    cout << "sort : Sort collection." << endl;
    cout << "sql : Get SQL." << endl;
    cout << "help : Help (print this menu)." << endl;
    cout << "quit : Quit." << endl;
}

void run_tests(QueryManager_ptr queryManager)
{
  printHelp();
  char buf[256];
  while (true){
    cout << "Command: ";
    cin.getline(buf, 100, '\n');
    logger.debugs() << "Command : " << buf << endl;
    if (!strcmp(buf, "create")) { 
        create_collection(queryManager);
        continue;
    }else if (!strcmp(buf, "subcollection")) { 
        create_subcollection();
        continue;
    }else if (!strcmp(buf, "subcollection_by_pattern")) { 
        create_subcollection_by_pattern();
        continue;
    }else if(!strcmp(buf,"destroy")) {
        destroy_collection();
        continue; 
    }else if(!strcmp(buf,"description")) {
        collection_get_record_description();
        continue; 
    }else if(!strcmp(buf,"set_readonly")) {
        collection_set_read_only();
        continue; 
    }else if(!strcmp(buf,"get_readonly")) {
        collection_get_read_only();
        continue; 
    }else if(!strcmp(buf,"count")) {
        collection_get_number_of_records();
        continue; 
    }else if(!strcmp(buf,"filter")) {
        collection_retrieve_by_filter();
        continue; 
    }else if(!strcmp(buf,"pattern")) {
        collection_retrieve_by_pattern();
        continue; 
    }else if(!strcmp(buf,"delete_all")) {
        collection_remove_all_records();
        continue; 
    }else if(!strcmp(buf,"delete")) {
        collection_remove_record();
        continue; 
    }else if(!strcmp(buf,"sort")) {
        collection_sort();
        continue; 
    }else if(!strcmp(buf,"sql")) {
        collection_get_sql();
        continue; 
    }else if(!strcmp(buf,"add")) {
        collection_add_record();
        continue; 
    }else if(!strcmp(buf,"add_rc")) {
        collection_add_rc();
        continue; 
    }else if(!strcmp(buf,"update_by_filter")) {
        collection_update_by_filter();
        continue; 
    }else if(!strcmp(buf,"update_by_pattern")) {
        collection_update_by_pattern();
        continue; 
    }else if(!strcmp(buf,"delete_by_filter")) {
        collection_remove_records_by_filter();
        continue; 
    }else if(!strcmp(buf,"delete_by_pattern")) {
        collection_remove_records_by_pattern();
        continue; 
    }else if(!strcmp(buf,"iterator")) {
        create_iterator();
        continue; 
    }else if(!strcmp(buf,"iterator_by_pattern")) {
        create_iterator_by_pattern();
        continue; 
    }else if(!strcmp(buf,"destroy_iterator")) {
        destroy_iterator();
        continue; 
    }else if(!strcmp(buf,"iterator_fetch_rc")) {
        iterator_fetch_rc();
        continue; 
    }else if(!strcmp(buf,"iterator_fetch_records")) {
        iterator_fetch_records();
        continue; 
    }else if(!strcmp(buf,"iterator_skip")) {
        iterator_skip();
        continue; 
    }else if(!strcmp(buf,"create_by_sql")) {
        create_collection_by_sql(queryManager);
        continue; 
    }else if(!strcmp(buf,"help")) {
        printHelp();
        continue; 
    }else if(!strcmp(buf,"quit")) {
        break;
    }else if(!strcmp(buf,"create_key_by_sql")) {
        create_key_collection_by_sql(queryManager);
        continue; 
    }else if(!strcmp(buf,"create_key")) {
        create_key_collection(queryManager);
        continue; 
    }else if(!strcmp(buf,"keydescription")) {
        get_key_description();
        continue; 
    }else if(!strcmp(buf,"get_record_with_key")) {
        retrieve_record_with_key();
        continue; 
    }else if(!strcmp(buf,"update_record_with_key")) {
        update_record_with_key();
        continue; 
    }else if(!strcmp(buf,"remove_record_with_key")) {
        remove_record_with_key();
        continue; 
    }else if(!strcmp(buf,"get_records_with_keys")) {
        retrieve_records_with_keys();
        continue; 
    }else if(!strcmp(buf,"update_records_with_keys")) {
        update_records_with_keys();
        continue; 
    }else if(!strcmp(buf,"remove_records_with_keys")) {
        remove_records_with_keys();
        continue; 
    }
  }
}

void create_collection(QueryManager_ptr qm_ptr)
{
  destroy_collection();
  created = false;
  created_as_key = false;
  logger.infos() << "CREATE Collection" << endl;
  cout << "Enter select part : ";
  char buff[100];
  cin.getline(buff, 100, '\n');
  logger.debugs() << "Select_part = " << buff << endl;
  select_part = string_dup(buff);
  cout << "Enter from part : ";
  cin.getline(buff, 100, '\n');
  logger.debugs() << "From_part = " << buff << endl;
  from_part = string_dup(buff);
  cout << "Enter where part : ";
  cin.getline(buff, 100, '\n');
  logger.debugs() << "Where_part = " << buff << endl;
  where_part = string_dup(buff);
  cout << "Enter order part : ";
  cin.getline(buff, 100, '\n');
  logger.debugs() << "Order_part = " << buff << endl;
  order_part = string_dup(buff);
  try {
    collection = 
         qm_ptr->create_collection_by_parts(select_part, from_part, where_part, order_part);
    created = true;
    logger.infos() << "Collection successfuly created." << endl;
  }catch(const QueryProcessingError& ex){
    created = false;
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const COMM_FAILURE& ex2){
    logger.fatals() << "COMM_FAILURE" << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    created = false;
    logger.errors() << "Operation : qm_ptr->create_collection_by_parts("
      << select_part << ", " << from_part << ", " << where_part<< ", "
      << order_part << ");"  << endl << "Error : " ;
      printSystemException(logger.errors(), ex);
      logger.errors() << endl;
    return;
  }
}

void create_collection_by_sql(QueryManager_ptr qm_ptr)
{
  destroy_collection();
  created = false;
  created_as_key = false;
  logger.infos() << "CREATE Collection by SQL" << endl;
  cout << "Enter SQL : ";
  char buff[1000];
  cin.getline(buff, 1000, '\n');
  logger.debugs() << "SQL= " << buff << endl;
  try {
    collection = 
         qm_ptr->create_collection(buff);
    created = true;
    logger.infos() << "Collection successfuly created." << endl;
  }catch(const QueryProcessingError& ex){
    created = false;
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << "COMM_FAILURE" << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    created = false;
    logger.errors() << "Operation : qm_ptr->create_collection("
      << buff << ");"  << endl << "Error : " ;
      printSystemException(logger.errors(), ex);
      logger.errors() << endl;
    return;
  }
}

void create_key_collection(QueryManager_ptr qm_ptr)
{
  destroy_collection();
  created = false;
  logger.infos() << "CREATE KeyCollection" << endl;
  cout << "Enter select part : ";
  char buff[100];
  cin.getline(buff, 100, '\n');
  logger.debugs() << "Select_part = " << buff << endl;
  select_part = string_dup(buff);
  cout << "Enter from part : ";
  cin.getline(buff, 100, '\n');
  logger.debugs() << "From_part = " << buff << endl;
  from_part = string_dup(buff);
  cout << "Enter where part : ";
  cin.getline(buff, 100, '\n');
  logger.debugs() << "Where_part = " << buff << endl;
  where_part = string_dup(buff);
  cout << "Enter order part : ";
  cin.getline(buff, 100, '\n');
  logger.debugs() << "Order_part = " << buff << endl;
  order_part = string_dup(buff);
  cout << "Enter key part : ";
  cin.getline(buff, 100, '\n');
  logger.debugs() << "Key_part = " << buff << endl;
  key_part = string_dup(buff);
  try {
    collection = 
         qm_ptr->create_key_collection_by_parts(select_part, from_part, where_part, order_part, key_part);
    created = true;
    created_as_key = true;
    logger.infos() << "KeyCollection successfuly created." << endl;
  }catch(const QueryProcessingError& ex){
    created = false;
    created_as_key = false;
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << "COMM_FAILURE" << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    created = false;
    created_as_key = false;
    logger.errors() << "Operation : qm_ptr->create_key_collection_by_parts("
      << select_part << ", " << from_part << ", " << where_part<< ", "
      << order_part << ", " << key_part << ");"  << endl << "Error : " ;
      printSystemException(logger.errors(), ex);
      logger.errors() << endl;
    return;
  }
}

void create_key_collection_by_sql(QueryManager_ptr qm_ptr)
{
  destroy_collection();
  created = false;
  created_as_key = false;
  logger.infos() << "CREATE KeyCollection by SQL" << endl;
  cout << "Enter SQL : ";
  char buff[1000];
  cin.getline(buff, 1000, '\n');
  logger.debugs() << "SQL= " << buff << endl;
  try {
    collection = 
         qm_ptr->create_key_collection(buff);
    created = true;
    created_as_key = true;
    logger.infos() << "KeyCollection successfuly created." << endl;
  }catch(const QueryProcessingError& ex){
    created = false;
    created_as_key = false;
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << "CORBA::COMM_FAILURE" << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    created = false;
    created_as_key = false;
    logger.errors() << "Operation : qm_ptr->create_key_collection("
      << buff << ");"  << endl << "Error : " ;
      printSystemException(logger.errors(), ex);
      logger.errors() << endl;
    return;
  }
}


void destroy_collection()
{
  logger.infos() << "DESTROY Collection" << endl;
  try {
    if (!created || collection==NULL){
      logger.infos() << "Collection not created." << endl;
      return;
    }
    created = false;
    collection->destroy();
    collection = NULL;
    destroy_iterator();
    logger.infos() << "Collection successfuly destroyed." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    printSystemException(logger.fatals(),ex2);
    logger.fatals() << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void create_iterator()
{
  logger.infos() << "Create iterator." << endl;
  try{
    if (!created || collection==NULL){
      logger.infos() << "At first create UAKGCollection" << endl;
      return;
    }
    iter = collection->create_iterator();
    created_iterator = true;
    logger.infos() << "Iterator successfuly created." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void create_iterator_by_pattern()
{
  logger.infos() << "Create iterator by pattern." << endl;
  try{
    if (!created || collection==NULL){
      logger.infos() << "At first create UAKGCollection" << endl;
      return;
    }
    RecordDescription_var descr = collection->getRecordDescription();
    Record_var pattern = get_record(descr);
    iter = collection->create_iterator_by_pattern(pattern);
    created_iterator = true;
    logger.infos() << "Iterator successfuly created." << endl;
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const ElementInvalid){
    logger.errors() << "Invalid element." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void destroy_iterator()
{
  if (!created_iterator || iter==NULL){
    logger.infos() << "Iterator already destroyed." << endl;
    return;
  }
  logger.infos() << "Destroy iterator." << endl;
  try{
    created_iterator = false;
    iter->destroy();
    iter = NULL;
    logger.infos() << "Iterator successfuly destroyed." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void collection_get_record_description()
{
  logger.infos() << "Get record description." << endl;
  cout << "Get record description." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    RecordDescription_var descr = collection->getRecordDescription();
    logger.infos() << "Results of getRecordDescription : " << endl;
    printRecordDescription(logger.infos(), descr);
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_set_read_only()
{
  logger.infos() << "Set read only." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  bool mode = false;
  try{
    cout << "Enter mode to switch (on/off default off):";
    char buffer[4];
    cin.getline(buffer, 4, '\n');
    logger.debugs() << "Switch readonly made to " << buffer << endl;
    if (!strcmp(buffer, "on"))
      mode = true;
    collection->set_readonly(mode);
  }catch(const ReadOnlyCollection){
    logger.errors() << "Error: ReadOnlyCollection." << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_get_read_only()
{
  logger.infos() << "Get read only." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    bool mode = collection->is_readonly();
    logger.infos() << "Current instance of collection is ";
    if (mode){
      logger.infos() << "\"readonly\".";
    } else {
      logger.infos() << "\"not readonly\".";
    }
    logger.infos() << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_get_number_of_records()
{
  logger.infos() << "Get number of records." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    unsigned long number_of_records = collection->get_number_of_records();
    logger.infos() << "Result of get number of records = " << number_of_records << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_retrieve_by_filter()
{
  logger.infos() << "Retrieve by filter." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  String_var filter;
  try{
    cout << "Enter filter : ";
    char buff[100];
    cin.getline(buff, 100, '\n');
    logger.debugs() << "Filter = " << buff << endl;
    filter=string_dup(buff);
    OctSeq_var rc = collection->retrieve_by_filter(filter);
    logger.infos() << "before printRC:" << endl;
    printRC(logger.infos(), rc);
    logger.infos() << endl;

  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_retrieve_by_pattern()
{
  logger.infos() << "Retrieve by pattern." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    RecordDescription_var descr = collection->getRecordDescription();
    Record_var pattern = get_record(descr);
    OctSeq_var rc = collection->retrieve_by_pattern(pattern);
    printRC(logger.infos(), rc);
    logger.infos() << endl;
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_remove_record()
{
  logger.infos() << "Delete record." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    RecordDescription_var descr = collection->getRecordDescription();
    Record_var record = get_record(descr);
    collection->remove_record(record);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const ReadOnlyCollection) {
    logger.errors() << "ReadOnly collection." << endl;
  }catch(const ElementInvalid) {
    logger.errors() << "Invalid element." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_remove_all_records()
{
  logger.infos() << "Delete all records." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    collection->remove_all_records();
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const ReadOnlyCollection) {
    logger.errors() << "ReadOnly collection." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_sort()
{
  logger.infos() << "Sort collection." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  String_var sort_part;
  try{
    cout << "Enter order expression : ";
    char buff[100];
    cin.getline(buff, 100, '\n');
    logger.debugs() << "Order = " << buff << endl;
    sort_part = string_dup(buff);
    collection->sort(sort_part);
    logger.infos() << "Collection successfuly sorted." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_get_sql()
{
  logger.infos() << "Get SQL." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    String_var s = collection->selectQueryText();
    logger.infos() << "SQL = " << s << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_add_record()
{
  logger.infos() << "Add record." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    RecordDescription_var descr = collection->getRecordDescription();
    Record_var record = get_record(descr);
    collection->add_record(record);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const ReadOnlyCollection) {
    logger.errors() << "ReadOnly collection." << endl;
  }catch(const ElementInvalid) {
    logger.errors() << "Invalid element." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_add_rc()
{
  logger.infos() << "Add record in RC format." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    RecordDescription_var descr = collection->getRecordDescription();
    Record_var record = get_record(descr);
    OctSeq_var octSeq = new OctSeq;
    octSeq->length(0);
    CORBA::ULong pos = 0;
    RCWriter::writeHead(pos,octSeq.inout());
    CORBA::ULong sv_pos = pos;
    RCWriter::writeNumberOfRecords(0,pos,octSeq.inout());
    RCWriter::writeNumberOfFields(descr->length(),pos,octSeq.inout());
    RCWriter::writeRecord(record,pos,octSeq.inout()); 
    RCWriter::writeNumberOfRecords(1,sv_pos,octSeq.inout());
    collection->add_rc(octSeq);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const ReadOnlyCollection) {
    logger.errors() << "ReadOnly collection." << endl;
  }catch(const ElementInvalid) {
    logger.errors() << "Invalid element." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_update_by_filter()
{
  logger.infos() << "Update record by filter." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    RecordDescription_var descr = collection->getRecordDescription();
    logger.infos() << "NEW RECORD:" << endl;
    Record_var record = get_record(descr);
    char buffer[100];
    cout << "FILTER: ";
    cin.getline(buffer, 100, '\n');
    logger.debugs() << "filter=" << buffer << endl;
    collection->update_by_filter(record, buffer);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const ReadOnlyCollection) {
    logger.errors() << "ReadOnly collection." << endl;
  }catch(const ElementInvalid) {
    logger.errors() << "Invalid element." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_update_by_pattern()
{
  logger.infos() << "Update record by pattern." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    RecordDescription_var descr = collection->getRecordDescription();
    logger.infos() << "NEW RECORD:" << endl;
    Record_var record = get_record(descr);
    logger.infos() << "PATTERN RECORD:" << endl;
    Record_var pattern = get_record(descr);
    collection->update_by_pattern(record, pattern);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const ReadOnlyCollection) {
    logger.errors() << "ReadOnly collection." << endl;
  }catch(const ElementInvalid) {
    logger.errors() << "Invalid element." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_remove_records_by_filter()
{
  logger.infos() << "Remove records by filter." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    char buffer[100];
    cout << "Filte: " << endl;
    cin.getline(buffer, 100, '\n');
    logger.debugs() << "Filter = " << buffer << endl;
    collection->remove_records_by_filter(buffer);
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const ReadOnlyCollection) {
    logger.errors() << "ReadOnly collection." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void collection_remove_records_by_pattern()
{
  logger.infos() << "Remove records by pattern." << endl;
  if (!created || collection==NULL){
    logger.infos() << "At first create UAKGCollection" << endl;
    return;
  }
  try{
    RecordDescription_var descr = collection->getRecordDescription();
    cout << "PATTERN RECORD:" << endl;
    Record_var pattern = get_record(descr);
    collection->remove_records_by_pattern(pattern);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const ReadOnlyCollection) {
    logger.errors() << "ReadOnly collection." << endl;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void iterator_fetch_rc()
{
  logger.infos() << "Iterator fetch RC." << endl;
  if (!created_iterator || iter==NULL){
    logger.infos() << "At first create UAKGIterator" << endl;
    return;
  }
  try{
    char buffer[100];
    cout << "Number of records to fetch: ";
    cin.getline(buffer, 100, '\n');
    CORBA::Boolean more = true;
    CORBA::ULong n = atoi(buffer);
    logger.debugs() << "Number = " << n << endl;
    OctSeq_var rc = iter->fetch_rc(n, more);
  CORBA::ULong pos = 0;
  RCHeader head;
  RCReader::readHeader(head,pos,rc.in());
  RecordSeq_var records;
  records = RCReader::readRecordSeq(pos,rc.in());

  printRecordSeq(cout, records);
//    printRC(cout, rc);
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void iterator_fetch_records()
{
  logger.infos() << "Iterator fetch records." << endl;
  if (!created_iterator || iter==NULL){
    logger.infos() << "At first create UAKGIterator" << endl;
    return;
  }
  try{
    char buffer[100];
    cout << "Number of records to fetch: ";
    cin.getline(buffer, 100, '\n');
    CORBA::Boolean more = true;
    CORBA::ULong n = atoi(buffer);
    logger.debugs() << "Number = " << n << endl;
    RecordSeq_var records = iter->fetch_records(n, more);
    printRecordSeq(cout, records);
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void iterator_skip()
{
  logger.infos() << "Iterator skip records." << endl;
  if (!created_iterator || iter==NULL){
    logger.infos() << "At first create UAKGIterator" << endl;
    return;
  }
  try{
    char buffer[100];
    cout << "Number of records to skip: ";
    cin.getline(buffer, 100, '\n');
    CORBA::Boolean more = true;
    CORBA::ULong n = atoi(buffer);
    logger.debugs() << "Number = " << n << endl;
    iter->skip(n, more);
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
    return;
  }
}

void create_subcollection()
{
  logger.infos() << "Create subcollection." << endl;
  try{
    if (!created || collection==NULL){
      logger.infos() << "At first create UAKGCollection" << endl;
      return;
    }
    char buffer[100];
    cout << "Filter: ";
    cin.getline(buffer,100,'\n');
    logger.debugs() << "Filter = " << buffer << endl;
    collection = collection->create_subcollection(buffer);
    logger.infos() << "Subcollection successfuly created." << endl;
  }catch(const QueryProcessingError& ex){
    created = false;
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const QueryInvalid& ex2){
    created = false;
    logger.errors() << "QueryInvalid:" << ex2.why << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    created = false;
    logger.errors() << ex << endl;
  }
}

void create_subcollection_by_pattern()
{
  logger.infos() << "Create subcollection by pattern." << endl;
  try{
    if (!created || collection==NULL){
      logger.infos() << "At first create UAKGCollection" << endl;
      return;
    }
    RecordDescription_var descr = collection->getRecordDescription();
    cout << "PATTERN:" << endl;
    Record_var pattern = get_record(descr);
    collection = collection->create_subcollection_by_pattern(pattern);
    logger.infos() << "Subcollection successfuly created." << endl;
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    created = false;
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const QueryInvalid){
    created = false;
    logger.errors() << "QueryInvalid." << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    created = false;
    logger.errors() << ex << endl;
  }
}

Record* get_record(RecordDescription_var descr)
{
  try{
    char buffer[100];
    Record_var record = new Record();
    record->length(descr->length());
    for(CORBA::ULong i=0; i<descr->length(); ++i) {
      printFieldDescription(cout,descr[i]);
      cout << " = ";
      cin.getline(buffer, 100, '\n');
      printFieldDescription(logger.debugs(), descr[i]);
      logger.debugs() << " = " << buffer << endl;
      if (strlen(buffer)==0){
        FieldValueAccess::setNull(record[i]);
      } else {
        cerr << "before setAsString, s=" << buffer << endl;
        cerr << "descr[" << i << "].type=" << descr[i].type << endl;
        FieldValueAccess::setAsString(record[i], buffer, descr[i].type);
        String_var s = FieldValueAccess::getAsString(record[i],"NULL");
        cerr << "after setAsString, s=" << s << endl;
        cerr << "type in string" << record[i]._d() << endl;
      }
    }
    return record._retn();
  }catch(InvalidFieldFormatException){
    logger.errors() << "Invalid field format." << endl;
    throw;
  }
}

OctSeq* records_to_rc(const RecordSeq& recseq, const RecordDescription& descr)
{
    OctSeq_var octSeq = new OctSeq;
    octSeq->length(0);
    CORBA::ULong pos = 0;
    RCWriter::writeHead(pos,octSeq.inout());
    RCWriter::writeNumberOfRecords(recseq.length(),pos,octSeq.inout());
    RCWriter::writeNumberOfFields(descr.length(),pos,octSeq.inout());
    RCWriter::writeRecordSeq(recseq,pos,octSeq.inout()); 
    return octSeq._retn();
}

void get_key_description()
{
  logger.infos() << "Get key description." << endl;
  try{
    if (!created || collection==NULL || !created_as_key){
      logger.infos() << "At first create UAKGKeyCollection" << endl;
      return;
    }
    UAKGKeyCollection_var keycollection = UAKGKeyCollection::_narrow(collection);
    RecordDescription_var descr = keycollection->get_key_description();
    logger.infos() << "Results of get_key_description : " << endl;
    printRecordDescription(logger.infos(), descr);
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void retrieve_record_with_key()
{
  logger.infos() << "Get record with key." << endl;
  try{
    if (!created || collection==NULL || !created_as_key){
      logger.infos() << "At first create UAKGKeyCollection" << endl;
      return;
    }
    UAKGKeyCollection_var keycollection = UAKGKeyCollection::_narrow(collection);
    RecordDescription_var keydescr = keycollection->get_key_description();
    Record_var keyrecord = get_record(keydescr);
    Record_var record = keycollection->retrieve_record_with_key(keyrecord);
    logger.infos() << "Results of retrieve_record_with_key : " << endl;
    printRecord(logger.infos(), record);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const QueryInvalid){
    logger.errors() << "QueryInvalid." << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void update_record_with_key()
{
  logger.infos() << "Update record with key." << endl;
  try{
    if (!created || collection==NULL || !created_as_key){
      logger.infos() << "At first create UAKGKeyCollection" << endl;
      return;
    }
    UAKGKeyCollection_var keycollection = UAKGKeyCollection::_narrow(collection);
    logger.infos() << "KEY RECORD : " << endl;
    RecordDescription_var keydescr = keycollection->get_key_description();   
    Record_var keyrecord = get_record(keydescr);
    logger.infos() << "NEW RECORD : " << endl;
    RecordDescription_var descr = keycollection->getRecordDescription();
    Record_var record = get_record(descr);
    keycollection->update_record_with_key(record, keyrecord);
    logger.infos() << "Record updated." << endl;
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const QueryInvalid){
    logger.errors() << "QueryInvalid." << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void remove_record_with_key()
{
  logger.infos() << "Remove record with key." << endl;
  try{
    if (!created || collection==NULL || !created_as_key){
      logger.infos() << "At first create UAKGKeyCollection" << endl;
      return;
    }
    UAKGKeyCollection_var keycollection = UAKGKeyCollection::_narrow(collection);
    logger.infos() << "KEY RECORD : " << endl;
    RecordDescription_var keydescr = keycollection->get_key_description();   
    Record_var keyrecord = get_record(keydescr);
    keycollection->remove_record_with_key(keyrecord);
    logger.infos() << "Record removed." << endl;
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const QueryInvalid){
    logger.errors() << "QueryInvalid." << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void retrieve_records_with_keys()
{
  logger.infos() << "Get records with keys." << endl;
  try{
    if (!created || collection==NULL || !created_as_key){
      logger.infos() << "At first create UAKGKeyCollection" << endl;
      return;
    }
    cout << "Enter number of keys:";
    char buffer[100];
    cin.getline(buffer, 100, '\n');
    int key_count = atoi(buffer);
    RecordSeq_var recseq = new RecordSeq();
    recseq->length(key_count);
    UAKGKeyCollection_var keycollection = UAKGKeyCollection::_narrow(collection);
    RecordDescription_var keydescr = keycollection->get_key_description();
    for (int i=0; i<key_count; i++){
       Record_var r = get_record(keydescr);
       recseq[i] = r;
    }
    OctSeq_var keys = records_to_rc(recseq.in(), keydescr.in());
    OctSeq_var rc = keycollection->retrieve_records_with_keys(keys);
    logger.infos() << "Results of retrieve_records_with_keys : " << endl;
    printRC(logger.infos(), rc);
    logger.infos() << endl;
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const QueryInvalid){
    logger.errors() << "QueryInvalid." << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void update_records_with_keys()
{
  logger.infos() << "Update records with keys." << endl;
  try{
    if (!created || collection==NULL || !created_as_key){
      logger.infos() << "At first create UAKGKeyCollection" << endl;
      return;
    }
    cout << "Enter number of records:";
    char buffer[100];
    cin.getline(buffer, 100, '\n');
    int records_count = atoi(buffer);
    RecordSeq_var recseq = new RecordSeq();
    recseq->length(records_count);
    UAKGKeyCollection_var keycollection = UAKGKeyCollection::_narrow(collection);
    RecordDescription_var descr = keycollection->getRecordDescription();
    for (int i=0; i<records_count; i++){
       Record_var r = get_record(descr);
       recseq[i] = r;
    }
    OctSeq_var rc = records_to_rc(recseq.in(), descr.in());
    keycollection->update_records_with_keys(rc);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const QueryInvalid){
    logger.errors() << "QueryInvalid." << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void remove_records_with_keys()
{
  logger.infos() << "Remove records with keys." << endl;
  try{
    if (!created || collection==NULL || !created_as_key){
      logger.infos() << "At first create UAKGKeyCollection" << endl;
      return;
    }
    cout << "Enter number of keys:";
    char buffer[100];
    cin.getline(buffer, 100, '\n');
    int key_count = atoi(buffer);
    RecordSeq_var recseq = new RecordSeq();
    recseq->length(key_count);
    UAKGKeyCollection_var keycollection = UAKGKeyCollection::_narrow(collection);
    RecordDescription_var keydescr = keycollection->get_key_description();
    for (int i=0; i<key_count; i++){
       Record_var r = get_record(keydescr);
       recseq[i] = r;
    }
    OctSeq_var keys = records_to_rc(recseq.in(), keydescr.in());
    keycollection->remove_records_with_keys(keys);
  }catch(InvalidFieldFormatException){
    logger.errors() << "Operation failed." << endl;
  }catch(const QueryProcessingError& ex){
    logger.errors() << "QueryProcessingError:" << ex.why << endl;
    return;
  }catch(const QueryInvalid){
    logger.errors() << "QueryInvalid." << endl;
    return;
  }catch(const CORBA::COMM_FAILURE& ex2){
    logger.fatals() << ex2 << endl;
    exit(1);
  }catch(const CORBA::SystemException& ex){
    logger.errors() << ex << endl;
  }
}

void printSystemException(ostream& os,const CORBA::SystemException& ex)
{
    os << ex << endl;
}

