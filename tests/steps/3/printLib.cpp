#include <QueryStepsPostConfig.h>
#include <importCORBA.h>
#include <importUAKGQuery.h>

#include <../3/printLib.h>

#include <iomanip>
using namespace std;
using namespace UAKGQuery2;

void printRecordSet(std::ostream& out, const UAKGQuery2::RecordSet* recordSet)
{
 RecordSet* rs = const_cast<RecordSet*>(recordSet);
 for(CORBA::ULong row=0; row<rs->getNRows(); ++row) {
   out <<'|';
   for(CORBA::ULong col=0; col<rs->getNColumns(); ++col) {
        if ( rs->isNullAt(row,col) ) {
            out << "Null|";
            continue;
        }
        switch ( rs->getFieldTypeAt(col) ) {
            case TypeNull:
                   out << "NULL" ;
                   break;
            case TypeBoolean:
                   out << ( rs->getBooleanAt(row,col) ? 'T' : 'F' );
                   break;
            case TypeChar:
                   out << rs->getCharAt(row,col);
                   break;
            case TypeOctet:
                   out << rs->getOctetAt(row,col);
                   break;
            case TypeShort:
                   out << rs->getShortAt(row,col);
                   break;
            case TypeUShort:
                   out << rs->getUShortAt(row,col);
                   break;
            case TypeLong:
                   out << rs->getLongAt(row,col);
                   break;
            case TypeULong:
                   out << rs->getULongAt(row,col);
                   break;
            case TypeFloat:
                   out << rs->getFloatAt(row,col);
                   break;
            case TypeDouble:
                   out << rs->getDoubleAt(row,col);
                   break;
            case TypeString:
                   {
                      CORBA::String_var str = rs->getStringAt(row,col);
                      out << str.in();
                   }
                   break;
            case TypeSmallInt:
                   out << rs->getShortAt(row,col);
                   break;
            case TypeInteger:
                   out << rs->getLongAt(row,col);
                   break;
            case TypeReal:
                   out << rs->getFloatAt(row,col);
                   break;
            case TypeDoublePrecision:
                   out << rs->getDoubleAt(row,col);
                   break;
            case TypeCharacter:
                   out << rs->getCharAt(row,col);
                   break;
            case TypeDecimal:
                   {
                       Numeric_var num = rs->getNumericAt(row,col);
                       printNumeric(out,num.in());
                   }
                   break;
            case TypeDateTime:
                   printDateTime(out, rs->getDateTimeAt(row,col));
                   break;
            case TypeNumeric:
                   {
                       Numeric_var  num = rs->getNumericAt(row,col);
                       printNumeric(out,num.in());
                   }
                   break;
            case TypeRaw:
                  {
                      UAKGQuery2::OctSeq_var oct = rs->getRawAt(row,col);
                      printRaw(out,oct);
                  }
                  break;
            case TypeBlob:
/*
                {
                   Blob_var bl = rs->getBlobAt(row,col);
                   CORBA::String_var str =
                      UAKGQuerySingleton::getORB_ptr()->object_to_string(bl);
                   out << "Blob_ior: " << str.in();
                }
*/
		out << "Blob"  << endl;
                break;
            case TypeClob:
/*
                  {
                   Clob_var cl = rs->getClobAt(row,col);
                   CORBA::String_var str =
                         UAKGQuerySingleton::getORB_ptr()->object_to_string(cl);                    out << "Clob_ior: " << str.in();
                  }
*/
		  out << "Clob" << endl;
                  break;
            case TypeWString:
                  {
                   CORBA::String_var s = (const char*)
                                rs->getWStringAt(row,col);
                   out << s;
                  }
                  break;
            case TypeWclob:
/*
                  {
                   Wclob_var wcl = rs->getWclobAt(row,col);
                   CORBA::String_var str =
                       UAKGQuerySingleton::getORB_ptr()->object_to_string(wcl);
                   out << "Wclob_ior: " << str.in();
                  }
*/
		  out << "Wclob" << endl;
                  break;
            default:
                  out << "UnknownFieldType : <" << rs->getFieldTypeAt(col) << ">";        } // switch
        out << '|';
   }    
   out << endl;
 }
}

void printNumeric(ostream& out, const Numeric* num)
{
  Numeric* ptr = const_cast<Numeric*>(num);
  // CORBA::String_var s = CORBA::string_dup( ptr->getAsString() );
  CORBA::Double s = ptr->getAsDouble();
  out << s;
}

void printDateTime(ostream& out, const DateTime& dt)
{
  out << setw(2) << (int)dt.day << ':' <<
                     (int)dt.month << ':' <<
         setw(4) << (int)dt.year << ':' <<
         setw(2) << (int)dt.hour << ':' <<
                     (int)dt.minute << ':' <<
                     (int)dt.second;
}

void printRaw(ostream& out,const OctSeq* octSeq)
{
 for(CORBA::ULong i=0; i<octSeq->length(); ++i){
   int x=(int)((*octSeq)[i]);
   out << setw(2) << '[' << x << ']' ;
 }
}

