#define UAKGQUERY_2
#define CORBA_H <OB/CORBA.h>
#define HAVE_TYPENAME 1
#define HAVE_NAMESPACE_STD 1
#define CORBA_HAVE_POA 1
#define CORBA_MODULE_NAMESPACE_MAPPING 1
#define HAVE_NAMESPACES 1
#define HAVE_BOOL 1
#define HAVE_CONFIG_H

#include <OB/CORBA.h>
#ifdef UAKGQUERY_1
#include <CosQuery.h>
#include <UAKGQuery.h>
#include <RC.h>
#include <UAKGQueryInit.h>
#include <CosQueryFacade/FieldValueAccess.h>
#include <CosQueryFacade/RecordAccess.h>
#include <CosQueryFacade/RecordDescriptionAccess.h>       
#include <CosQueryFacade/DecimalAccess.h>       
#endif

#ifdef UAKGQUERY_2
#include <importCORBA.h>
#include <importUAKGQuery.h>
#include <UAKGQuery2Init.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <string>
#include <strstream>
#include <iostream>
#include <iomanip>
#include <deque>

using namespace std;
#ifdef UAKGQUERY_1
using namespace CosQuery;
using namespace UAKGQuery;
using namespace RC;
using namespace CosQueryCollection;
#endif

#ifdef UAKGQUERY_2
using namespace UAKGQuery2;
#endif

void  init(void);
void  test_query(QueryManager_ptr queryManager);
char* get_string(void);
#ifdef UAKGQUERY_1
void  put_data( RecordSeq_var& inpRecordSeq, CORBA::ULong row,
                FieldType  fieldtype, CORBA::ULong nrec = 0);
#endif
#ifdef UAKGQUERY_2
void  put_data( RecordSet*& rs, Query_ptr q, CORBA::ULong row, CORBA::ULong col,                FieldType fieldtype, CORBA::ULong nrec = 0);
#endif
void  query_insert(QueryManager_ptr queryManager, const int nrows = 1);
void  query_select(QueryManager_ptr queryManager);
void  query_update(QueryManager_ptr queryManager);
void  query_delete(QueryManager_ptr queryManager);
void  query_delete_all( QueryManager_ptr queryManager );
int   parse_options(int argc, char* argv[]);
void  usage();

#ifdef UAKGQUERY_2
RecordSet_init* recordSetFactory;
Numeric_init*   numericFactory;
#endif

class Element {
     CORBA::Long tlong_;
     CORBA::Double tdouble_;
     char* tstring_;
     DateTime  tdatetime_;
     Numeric* tnumeric_;
public:
     Element(): tlong_(0), tdouble_(0), tstring_(NULL), 
				tdatetime_(), tnumeric_(NULL)
     {} 

     CORBA::Long  get_long() const { return tlong_; }
     void  set_long( const CORBA::ULong lg ) { tlong_ = lg; }

     CORBA::Double get_double() const { return tdouble_; }
     void set_double( const CORBA::Double dbl ) { tdouble_ = dbl; }

     const char*  get_string() const { return tstring_; }
     void  set_string( const char* str ) 
     { 
		if (tstring_ != NULL) {
			CORBA::string_free(tstring_);
			tstring_ = NULL;
		}
		tstring_ = CORBA::string_dup(str); 
     }

     DateTime  get_DateTime() const { return tdatetime_; }
     void      set_DateTime( const DateTime& dt ) { tdatetime_ = dt; }

     Numeric*  get_numeric() const { return tnumeric_; }
     void      set_numeric(Numeric* num)
     { 
		if (tnumeric_ != NULL) 
			tnumeric_->_remove_ref();
		if (num)
			num->_add_ref();
		tnumeric_ = num;
     }

     Element(const Element& el) : tlong_(el.tlong_),
				  tdouble_(el.tdouble_),
				  tdatetime_(el.tdatetime_),
				  tstring_(NULL), tnumeric_(NULL)
     { 
	 	set_string(el.tstring_);  set_numeric(el.tnumeric_);
     }

     void operator=(const Element& el)
     {
		if ( this != &el ) {
	   		set_long(el.tlong_);
	   		set_double(el.tdouble_);
	   		set_string(el.tstring_);
	   		set_DateTime(el.tdatetime_);
	   		set_numeric(el.tnumeric_);
		}
     }
    
     void destroy() 
     {
		if ( tstring_ )
	     	CORBA::string_free(tstring_); 
        tstring_ = NULL;
		if (tnumeric_)
	    	tnumeric_->_remove_ref(); 
        tnumeric_ = NULL;
     }

     ~Element() { destroy(); }
};

typedef deque<Element> ArrayOfElement;

static ArrayOfElement table_;
static unsigned  int  ntimes_ = 10;


int main(int argc, char* argv[])
{
  if ( parse_options( argc, argv ) ) {
	usage(); return 0;
  }
  try {
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
    initUAKGQuery2(orb.in());


#ifdef UAKGQUERY_2
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
#endif

    CORBA::Object_var obj;
    try {
	obj = orb->resolve_initial_references("UAKGQueryService2");
    } catch(const CORBA::ORB::InvalidName& ex){
	cerr << argv[0] << ": can't resolve UAKGQueryService" << endl;
	return 1;
    }

    if(CORBA::is_nil(obj)) {
	cerr << argv[0] 
	<< ": UAKGQueryService is a nil object reference"  << endl;
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
#ifdef UAKGQUERY_1
           dbManager->createQueryManager
#endif
#ifdef UAKGQUERY_2
           dbManager->create_query_manager
#endif
					("scott",
					 "tiger",
					 "trolldb",
					 "Oracle8","");
    }catch(QueryManagerNotFound){
      cerr << argv[0] <<": can't find query manager." << endl;
      return 1;
    }
    init();
    test_query( queryManager );
    query_delete_all( queryManager );
    cout << "disconnecting." << endl;
    queryManager->destroy();
    // dbManager->shutdown();
    orb->destroy();
 }catch(const CORBA::SystemException& ex){
   cerr << ex << endl;
   return 1;
 }
 return 0;
}

void  init(void)
{
      srand( time(NULL) );
}

void usage()
{
   cerr << " Usage:  QueryCrashTest [-n]" << endl;
}

int  parse_options(int argc, char* argv[])
{
   for( int i = 1; i < argc; ++i ) {
	if ( strcmp( argv[i], "-n" ) == 0 ) {
	    if ( argv[++i] ) {
		char* errptr = NULL;
		unsigned int n = 0;
		
		n = strtoul( argv[i], &errptr, 10);	
		if ( errptr != NULL ) ntimes_ = n;
	    }
	}
   }
   return 0; 
}

char* get_string(void)
{
    CORBA::ULong str_len = rand() % 64;
    char* str;
    str = CORBA::string_alloc( str_len + 1 );
    memset(str, '#' ,str_len);
    str[str_len] = '\0';
    return str;
}

void test_query(QueryManager_ptr queryManager)
{
     cerr << "Inserting " << ntimes_ << " rows" << endl;
     for( int i = 0; i < ntimes_; i++ ) {
		query_insert( queryManager );
     }
     cerr << "Selecting rows" << endl;
     query_select( queryManager );
     for( int i = 0; i < table_.size(); i++ ) {
	cout << i << ":" << table_[i].get_string() << endl;
     }
     sleep(1);
     cerr << "Updating rows" << endl;
     query_update( queryManager );
     sleep(1);
     cerr << "Selecting rows" << endl;
     query_select( queryManager );
     for( int i = 0; i < table_.size(); i++ ) {
	cout << i << ":" << table_[i].get_string() << endl;
     }
     cerr << "Deleting rows" << endl;
     query_delete( queryManager );
}

#ifdef UAKGQUERY_2
void  put_data( RecordSet*& rs, Query_ptr q, CORBA::ULong row, CORBA::ULong col,
                FieldType fieldtype, CORBA::ULong nrec = 0)
{
     switch ( fieldtype ) {
        case TypeLong:
		rs->setLongAt( row, col, 0);
                break;
        case TypeString: {
				CORBA::String_var str = get_string();
				rs->setStringAt(row, col, str.in());
			}
			break;
        case TypeDouble:
			rs->setDoubleAt(row, col, 0);
			break;
        case TypeDateTime: {
				time_t timev = time(0);
				struct tm stm;
				DateTime date_time;
				memcpy(&stm, localtime(&timev), sizeof(struct tm));
				date_time.year = stm.tm_year + 1900;
				date_time.month = stm.tm_mon + 1;
				date_time.day = stm.tm_mday;
				date_time.hour = stm.tm_hour;
				date_time.minute = stm.tm_min;
				date_time.second = stm.tm_sec;

				rs->setDateTimeAt(row, col, date_time);
			}
			break;
		case TypeNumeric: {
				Numeric_var num = numericFactory->create();
				num->setAsDouble(0.0);
				rs->setNumericAt(row, col, num);
			}
			break;
	    case TypeRaw: {
				OctSeq_var rawExl = new OctSeq();
				rawExl->length(20);
				rs->setRawAt(row, col, rawExl);
        	}
			break;
	    case TypeBlob: {
				OctSeq_var rawExl = new OctSeq();
				rawExl->length(20);
				Blob_var bl = q->create_blob();
				bl->put_chunk(rawExl.in());
				rs->setBlobAt(row, col, bl);
        	}
        	break;
     }
}

void    query_insert(QueryManager_ptr queryManager, const int nrows = 1)
{
  const char* sql = "INSERT INTO basic_types(tlong, tstring, tdouble, tdate, traw, tnumeric, tblob) VALUES (:tlng, :tstr, :tdbl, :tdt, :trw, :tnum, :tblb)";
  Query_var query = queryManager->create_query(sql,"");
  try {
	CORBA::ULong col;
	RecordSet_var rs = recordSetFactory->create();

	rs->addColumn(":tlng", TypeLong);
	col = rs->getNColumns();
	rs->addColumn(":tstr", TypeString);
	rs->setFieldSizeAt(col, 100);
	rs->addColumn(":tdbl", TypeDouble);
	rs->addColumn(":tdt", TypeDateTime);
	col = rs->getNColumns();
	rs->addColumn(":trw", TypeRaw);
	rs->setFieldSizeAt(col++, 100);
	rs->addColumn(":tnum",TypeNumeric);
	rs->setFieldSizeAt(col++, 100);
	rs->addColumn(":tblb",TypeBlob);
	rs->setFieldSizeAt(col, 100);

        query->prepare_query( rs );
	rs->addRow();
	
	for( int i = 0; i < nrows; ++i ) {
	    put_data(rs, query, 0, 0, TypeLong);
	    put_data(rs, query, 0, 1, TypeString);
	    put_data(rs, query, 0, 2, TypeDouble);
	    put_data(rs, query, 0, 3, TypeDateTime);
	    put_data(rs, query, 0, 4, TypeRaw);
	    put_data(rs, query, 0, 5, TypeNumeric);
	    try {
	      put_data(rs, query, 0, 6, TypeBlob);
	    } catch( const ReadOnlyLOB& ex ) {
	      cerr << "LOB only for reading" << endl;
	    }
	    query->execute(rs);
	}
  }catch( const InvalidFieldType&  ex){
   cerr << "InvalidFieldType" << endl;
  }catch(const QueryFlagInvalid& ex){
   cerr << "QueryFlagInvalid:" << ex.why << endl;
  }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
  }
  query->destroy();
}

void  query_select(QueryManager_ptr queryManager)
{
  const char* sql = "SELECT tlong, tstring, tdouble, tdate, tnumeric FROM basic_types";
  Query_var query = queryManager->create_query(sql,"");
  CORBA::Boolean more = true;
  try {
     RecordSet_var recordSet = recordSetFactory->create();
     query->prepare_query(recordSet);
     query->execute(recordSet);
     CORBA::Boolean more = false;
  do {

	RecordSet_var rs = recordSetFactory->create();
	rs = query->fetch(10, more);

	for( int row = 0; row<rs->getNRows(); row++) {
		Element el;
		for( int col = 0; col < rs->getNColumns(); col++ ) {
			try {
			  if ( rs->isNullAt(row,col) )
					continue;
			  switch (rs->getFieldTypeAt(col)) {
				case TypeNull: continue;
				case TypeLong: 
					el.set_long(rs->getLongAt(row, col));
					break;
				case TypeString: {
						CORBA::String_var str = rs->getStringAt(row, col);
						el.set_string(str);
			        }
					break;
				case TypeDouble:
					el.set_double(rs->getDoubleAt(row, col));
					break;
				case TypeDateTime:
					el.set_DateTime(rs->getDateTimeAt(row, col));
					break;
				case TypeNumeric: {
						Numeric_var num = numericFactory->create();
						num = rs->getNumericAt(row, col);
					el.set_numeric(num);
					}
					break;
			    case TypeBlob: {
						OctSeq_var oct = new OctSeq;
						Blob_var bl = query->create_blob();
						bl = rs->getBlobAt(row, col);
						CORBA::Boolean more = false;
						oct = bl->fetch_chunk(100, more);
					}
					break;
		   	   }
			} catch(const InvalidRow& ex) {
				cerr << "Invalid row" << endl;
			} catch(const InvalidColumn& ex) {
				cerr << "Invalid column" << endl;
			} catch(const FieldIsNull& ex) {
				cerr << "Field is Null" << endl;
			}catch(const InvalidFieldType&  ex){
				cerr << "InvalidFieldType" << endl;
			}catch(const WriteOnlyLOB& ex){
				cerr << "Blob only for writing" << endl;
			} 
	  	} //for
        table_.push_back(el); 
	} //for
  } while ( more );

	//
    // Blob read.
	//
    query->destroy();

	cout << "Selecting blob" << endl;
	const char* blob_sql = "SELECT tblob FROM basic_types";
    query = queryManager->create_query(blob_sql,"");
    more = false;
    query->execute_e();
	RecordSet_var rs = recordSetFactory->create();
	rs = query->fetch(1, more);
	for( int row = 0; row < rs->getNRows(); row++ ) {
		try{
			if ( rs->isNullAt(row,0) )
				continue;
			Blob_var bl = rs->getBlobAt(row,0);
			OctSeq_var octSeq;
			for(CORBA::Boolean exist = true; exist; ) {
				octSeq = bl->fetch_chunk(10, exist);
			}
		} catch (const InvalidFieldType& ex) {
			cerr << "InvalidFieldType" << endl;
			cerr << "FieldType is " 
				 << rs->getAsStringAt(0,0,"Null") << endl;
		} catch (const FieldIsNull& ) {
			cerr << "FieldIsNull" << endl;
		}
	}
  }catch( const InvalidFieldType&  ex){
	cerr << "InvalidFieldType" << endl;
  }catch(const QueryFlagInvalid& ex){
	cerr << "QueryFlagInvalid:" << ex.why << endl;
  }catch(const QueryProcessingError& ex){
	cerr << "QueryProcessingError" << endl;
	cerr << ex.why << endl;
  }
  query->destroy();
}


void  query_update(QueryManager_ptr queryManager)
{
  const char* sql = "UPDATE basic_types SET tlong=:tlng, tstring=:tstr, tdouble=:tdbl, tdate=:tdt, traw=:trw, tnumeric=:tnum WHERE tstring=:tsearch";
  Query_var query = queryManager->create_query(sql,"");
  try {
        CORBA::ULong col;
        RecordSet_var rs = recordSetFactory->create();

        rs->addColumn(":tlng", TypeLong);
        col = rs->getNColumns();
        rs->addColumn(":tstr", TypeString);
        rs->setFieldSizeAt(col, 100);
        rs->addColumn(":tdbl", TypeDouble);
        rs->addColumn(":tdt", TypeDateTime);
        col = rs->getNColumns();
        rs->addColumn(":trw", TypeRaw);
        rs->setFieldSizeAt(col++, 100);
        rs->addColumn(":tnum",TypeNumeric);
        rs->setFieldSizeAt(col++, 100);
        rs->addColumn(":tsearch", TypeString);
        rs->setFieldSizeAt(col, 100);

        query->prepare_query( rs );
        rs->addRow();

        for( int i = 0; i < table_.size(); ++i ) {
            put_data(rs, query, 0, 0, TypeLong);
            put_data(rs, query, 0, 1, TypeString);
            put_data(rs, query, 0, 2, TypeDouble);
            put_data(rs, query, 0, 3, TypeDateTime);
            put_data(rs, query, 0, 4, TypeRaw);
            put_data(rs, query, 0, 5, TypeNumeric);
			if ( table_[i].get_string() ) {
				const char* str;
				str = table_[i].get_string();
				rs->setStringAt( 0, 6, str );
				query->execute(rs);
			}
		}
  } catch(const InvalidRow& ex) {
   cerr << "Invalid row" << endl;
  } catch(const InvalidColumn& ex) {
   cerr << "Invalid column" << endl;
  }catch( const InvalidFieldType&  ex){
   cerr << "InvalidFieldType" << endl;
  }catch(const QueryFlagInvalid& ex){
   cerr << "QueryFlagInvalid:" << ex.why << endl;
  }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
  }
  query->destroy();
}

void  query_delete(QueryManager_ptr queryManager)
{
  const char* sql = "DELETE FROM basic_types  WHERE tstring=:tsearch";
  Query_var query = queryManager->create_query(sql,"");
  try {
     RecordSet_var rs = recordSetFactory->create();
     rs->addColumn(":tsearch",TypeString);
     rs->setFieldSizeAt(0, 100);
     query->prepare_query(rs);
     rs->addRow();
     for( int i = 0; i < table_.size(); i++ ) {
	if ( table_[i].get_string() != NULL ) {
	    rs->setStringAt(0, 0, table_[i].get_string());
	    query->execute(rs);
	}
     }
  }catch( const InvalidFieldType&  ex){
   cerr << "InvalidFieldType" << endl;
  }catch(const QueryFlagInvalid& ex){
   cerr << "QueryFlagInvalid:" << ex.why << endl;
  }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
  }
  query->destroy();
}

void  query_delete_all( QueryManager_ptr queryManager )
{
  try {
    RecordSet_var recordSet = queryManager->evaluate_e("DELETE FROM basic_types","");
  }catch( const InvalidFieldType&  ex){
   cerr << "InvalidFieldType" << endl;
  }catch(const QueryFlagInvalid& ex){
   cerr << "QueryFlagInvalid:" << ex.why << endl;
  }catch(const QueryProcessingError& ex){
   cerr << "QueryProcessingError" << endl;
   cerr << ex.why << endl;
  }
}
#endif
