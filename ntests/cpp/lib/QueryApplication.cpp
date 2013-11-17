/*
 * part of Gard-Soft Query Service.
 * (C) Grad-Soft Ltd. Kiev, Ukraine
 * http://www.gradsoft.kiev.ua
 * 2003
*/

#include <QueryApplication.h>
#include <UAKGQuery2Init.h>
#include <iomanip>

QueryApplication::QueryApplication()
{
 progOptions_.put("db-user","login of database user", true);
 progOptions_.put("db-password","login of database user", true);
 progOptions_.put("db-link","link to database",true);
 progOptions_.put("db-driver","database connection driver",true);
 progOptions_.put("db-flags","database connection flags",true);
 progOptions_.allowUnknownOptions();
}
    
QueryApplication::~QueryApplication()
{
}

int QueryApplication::run(int argc, char** argv)
{

 this->configure();

 if (!progOptions_.parse(argc,argv)) {
   return 1;
 }

 try {
#ifdef CORBA_ORB_INIT_HAVE_3_ARGS
    orb_ = CORBA::ORB_init(argc, argv, CORBA_ORB_INIT_THIRD_ARG);
#else
    orb_ = CORBA::ORB_init(argc, argv);
#endif
 }catch(const CORBA::SystemException& ex){
    std::cerr << "exception during ORB initialization" << std::endl;
    std::cerr << ex << std::endl;
	return 1;
 }

 try {
    initUAKGQuery2(orb_.in());
 }catch(const CORBA::SystemException& ex){
	 std::cerr << "exception during UAKGQuery2 initialization" << std::endl;
	 std::cerr << ex << std::endl;
	 return 1;
 }

 CORBA::Object_var obj;
 try {
    obj = orb_->resolve_initial_references("UAKGQueryService");
 }catch(const CORBA::ORB::InvalidName&){
	std::cerr << argv[0] << ": can't resolve UAKGQueryService" << std::endl;
    return 1;
 }
        
 if(CORBA::is_nil(obj)) {
	 std::cerr << argv[0] << ": UAKGQueryService is a nil object reference" << std::endl;
     return 1;
 }
 
 CORBA::ValueFactory vf=orb_->lookup_value_factory(
                             "IDL:gradsoft.kiev.ua/UAKGQuery2/RecordSet:1.0");
 recordSetFactory_=UAKGQuery2::RecordSet_init::_downcast(vf);
 if (recordSetFactory_==NULL) {
   std::cerr <<  "Internal error: can't find valuetype factory for RecordSet" << std::endl;
   return 1;
 }

 vf=orb_->lookup_value_factory("IDL:gradsoft.kiev.ua/UAKGQuery2/Numeric:1.0");
 numericFactory_=UAKGQuery2::Numeric_init::_downcast(vf);
 if (numericFactory_==NULL) {
   std::cerr <<  "Internal error: can't find valuetype factory for Numeric" << std::endl;
   return 1;
 }

 UAKGQuery2::DBConnectionManager_var dbManager;
 try {
	 dbManager = UAKGQuery2::DBConnectionManager::_narrow(obj);
 }catch(const CORBA::SystemException& ex){
	 std::cerr << "Can't narrow DBConnectionManager" << std::endl;
	 std::cerr << ex << std::endl;
 }

 if (CORBA::is_nil(dbManager)) {
	 std::cerr << argv[0] << ": can't narrow dbManager to correct type" << std::endl;
     return 1;
 } 

 const char* dbLogin;
 if (progOptions_.is_set("db-user")) {
	 dbLogin=progOptions_.argument("db-user");
 }else{
	 dbLogin="ntest";
 }

 const char* dbPassword;
 if (progOptions_.is_set("db-password")) {
	 dbPassword=progOptions_.argument("db-password");
 }else{
	 dbPassword="ntest";
 }

 const char* dbLink;
 if (progOptions_.is_set("db-link")) {
	 dbLink=progOptions_.argument("db-link");
 }else{
	 dbLink="";
 }

 const char* dbDriver;
 if (progOptions_.is_set("db-driver")) {
	 dbDriver=progOptions_.argument("db-driver");
 }else{
	 dbDriver=getDefaultDBDriver();
 }

 const char* dbFlags;
 if (progOptions_.is_set("db-flags")) {
	 dbFlags=progOptions_.argument("db-flags");
 }else{
	 dbFlags="";
 }

 bool validQueryManager=true;
 bool testResult=false;

 UAKGQuery2::QueryManager_var queryManager;
 try {
      queryManager = 
           dbManager->create_query_manager(dbLogin,dbPassword,dbLink,dbDriver,dbFlags);
 }catch(const UAKGQuery2::QueryProcessingError& ex){
	 std::cerr << argv[0] <<": can't create query manager:" << ex.why << std::endl;
     return 1;
 }catch(UAKGQuery2::QueryManagerNotFound){
	 std::cerr << argv[0] <<": can't find query manager." << std::endl;
     return 1;
 }catch(const CORBA::SystemException& ex){
	 std::cerr << argv[0] <<": error during creating query manager - " << ex << std::endl;
     return 1;
 }


 this->init();

 if (CORBA::is_nil(queryManager.in())) {
	 std::cerr << argv[0] <<": query manager is nil " << std::endl;
	 validQueryManager=false;
 }

 if (validQueryManager) {
   try {
     testResult=doQueryStuff(queryManager.in());
   }catch(const CORBA::SystemException& ex){
	 std::cerr << argv[0] << ":" << ex << std::endl;
   }catch(const UAKGQuery2::QueryProcessingError& ex){
	 std::cerr << argv[0] << ":" << ex.why << std::endl;
   }catch(const std::exception& ex){
	 std::cerr << argv[0] << ":" << ex.what() << std::endl;
   }

   //time to disconnect.
   //
   try {
     queryManager->destroy();
   }catch(const CORBA::SystemException& ex){
	 std::cerr << argv[0] << ":error during QueryManager::destroy" << std::endl;
	 std::cerr << argv[0] << ":" << ex << std::endl;
	 testResult=false;
   }
 }
  
 try {
   orb_->destroy();
 }catch(const CORBA::SystemException& ex){
	 std::cerr << argv[0] << ":error during ORB::destroy" << std::endl;
	 std::cerr << argv[0] << ":" << ex << std::endl;
	 testResult=false;
  }

 return (testResult==true) ? 0 : 1;

}

/*
 * must be overloaded to return database driver.
 *  (Oracle8 Oracle9 InterBase ODBC)
 */
const char*  QueryApplication::getDefaultDBDriver() const
{ return "Oracle8"; }

void QueryApplication::configure()
{}

void QueryApplication::init()
{}


    // what we call from subclasses:

UAKGQuery2::RecordSet*  QueryApplication::createRecordSet()
{
 return recordSetFactory_->create();
}

UAKGQuery2::Numeric*    QueryApplication::createNumeric()
{
 return numericFactory_->create();
}


GradSoft::ProgOptions&  QueryApplication::getProgOptions()
{
 return progOptions_;
}


void  QueryApplication::printRecordSet(std::ostream& out, UAKGQuery2::RecordSet* rs)
{
 for(CORBA::ULong row=0; row<rs->getNRows(); ++row) {
   out <<'|';
   for(CORBA::ULong col=0; col<rs->getNColumns(); ++col) {
        if ( rs->isNullAt(row,col) ) {
            out << "Null|";
            continue;
        }
        switch ( rs->getFieldTypeAt(col) ) {
			case UAKGQuery2::TypeNull:
                   out << "NULL" ;
                   break;
            case UAKGQuery2::TypeBoolean:
                   out << ( rs->getBooleanAt(row,col) ? 'T' : 'F' );
                   break;
            case UAKGQuery2::TypeChar:
                   out << rs->getCharAt(row,col);
                   break;
            case UAKGQuery2::TypeOctet:
                   out << rs->getOctetAt(row,col);
                   break;
            case UAKGQuery2::TypeShort:
                   out << rs->getShortAt(row,col);
                   break;
            case UAKGQuery2::TypeUShort:
                   out << rs->getUShortAt(row,col);
                   break;
            case UAKGQuery2::TypeLong:
                   out << rs->getLongAt(row,col);
                   break;
            case UAKGQuery2::TypeULong:
                   out << rs->getULongAt(row,col);
                   break;
            case UAKGQuery2::TypeLongLong:
                   out << rs->getLongLongAt(row,col);
                   break;
            case UAKGQuery2::TypeULongLong:
                   out << rs->getULongLongAt(row,col);
                   break;
            case UAKGQuery2::TypeFloat:
                   out << rs->getFloatAt(row,col);
                   break;
            case UAKGQuery2::TypeDouble:
                   out << rs->getDoubleAt(row,col);
                   break;
            case UAKGQuery2::TypeString:
                   {
                      CORBA::String_var str = rs->getStringAt(row,col);
                      out << str.in();
                   }
                   break;
            case UAKGQuery2::TypeSmallInt:
                   out << rs->getShortAt(row,col);
                   break;
            case UAKGQuery2::TypeInteger:
                   out << rs->getLongAt(row,col);
                   break;
            case UAKGQuery2::TypeReal:
                   out << rs->getFloatAt(row,col);
                   break;
            case UAKGQuery2::TypeDoublePrecision:
                   out << rs->getDoubleAt(row,col);
                   break;
            case UAKGQuery2::TypeCharacter:
                   out << rs->getCharAt(row,col);
                   break;
            case UAKGQuery2::TypeDecimal:
                   {
					   UAKGQuery2::Numeric_var num = rs->getNumericAt(row,col);
                       printNumeric(out,num.in());
                   }
                   break;
            case UAKGQuery2::TypeDateTime:
                   printDateTime(out, rs->getDateTimeAt(row,col));
                   break;
            case UAKGQuery2::TypeNumeric:
                   {
					   UAKGQuery2::Numeric_var  num = rs->getNumericAt(row,col);
                       printNumeric(out,num.in());
                   }
                   break;
            case UAKGQuery2::TypeRaw:
                  {
                      UAKGQuery2::OctSeq_var oct = rs->getRawAt(row,col);
                      printRaw(out,oct);
                  }
                  break;
            case UAKGQuery2::TypeBlob:
/*
                {
                   Blob_var bl = rs->getBlobAt(row,col);
                   CORBA::String_var str =
                      UAKGQuerySingleton::getORB_ptr()->object_to_string(bl);
                   out << "Blob_ior: " << str.in();
                }
*/
		        out << "Blob";
                break;
            case UAKGQuery2::TypeClob:
/*
                  {
                   Clob_var cl = rs->getClobAt(row,col);
                   CORBA::String_var str =
                         UAKGQuerySingleton::getORB_ptr()->object_to_string(cl);                    out << "Clob_ior: " << str.in();
                  }
*/
				out << "Clob";
                  break;
            case UAKGQuery2::TypeWString:
                  {
                   CORBA::String_var s = (const char*)
                                rs->getWStringAt(row,col);
                   out << s;
                  }
                  break;
			case UAKGQuery2::TypeWclob:
/*
                  {
                   Wclob_var wcl = rs->getWclobAt(row,col);
                   CORBA::String_var str =
                       UAKGQuerySingleton::getORB_ptr()->object_to_string(wcl);
                   out << "Wclob_ior: " << str.in();
                  }
*/
		          out << "Wclob";
                  break;
            default:
                  out << "UnknownFieldType : <" << rs->getFieldTypeAt(col) << ">";        } // switch
        out << '|';
   }    
   out << std::endl;
 }
}

void QueryApplication::printNumeric(std::ostream& out, UAKGQuery2::Numeric* num)
{
  CORBA::String_var s = num->getAsString();
  out << s.in();
}

void QueryApplication::printDateTime(std::ostream& out, const UAKGQuery2::DateTime& dt)
{
	out << std::setw(2) << (int)dt.day << ':' <<
                           (int)dt.month << ':' <<
		   std::setw(4) << (int)dt.year << ':' <<
		   std::setw(2) << (int)dt.hour << ':' <<
                           (int)dt.minute << ':' <<
                           (int)dt.second;
}

void QueryApplication::printRaw(std::ostream& out,const UAKGQuery2::OctSeq& octSeq)
{
 static char hex[]={ '0','1','2','3','4','5','6','7','8','9',
                     'A','B','C','D','E','F' };
 for(CORBA::ULong i=0; i<octSeq.length(); ++i){
   int x=(int)(octSeq[i]);
   out << std::setw(2) << '[' << hex[(x&0xF0)<<8] << hex[x&0x0F] << ']' ;
 }
}
