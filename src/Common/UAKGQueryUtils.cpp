#ifndef __Common_UAKGQueryUtils_h
#include <Common/UAKGQueryUtils.h>
#endif

/**
 * Part of GradSoft Query Service.
 * (C) Grad-Soft Ltd, Kiev, Ukraine.
 * $Id: UAKGQueryUtils.cpp,v 1.29 2003-11-12 13:11:42 rssh Exp $
 **/

#include <sstream>
#include <iomanip>
using namespace std;
using namespace UAKGQuery2;

#include <UAKGQuerySingleton.h>

StringSeq* UAKGQueryUtils::szArray2StringSeq(const char * const* szArray)
{
 StringSeq_var retval = new StringSeq;
 CORBA::ULong n=0;
 while(*szArray) {
   retval->length(n+1);
   retval[n]=CORBA::string_dup(*szArray);
   ++szArray;
 } 
 return retval._retn();
}

#ifdef CORBA_HAVE_OTS

void UAKGQueryUtils::otid_to_xid(const CosTransactions::otid_t& otid,
                                 XID& xid)
{
 xid.formatID = otid.format_id;
 CORBA::ULong length = otid.tid.length();
 xid.gtrid_length = length - otid.bqual_length;
 xid.bqual_length = otid.bqual_length;
 memset(xid.data,0,XIDDATASIZE);
 memcpy(xid.data,otid.tid.get_buffer(),length);
}

void UAKGQueryUtils::print_xid(ostream& out,const XID& xid)
{
  out << "[" << xid.formatID << ",";
  int i=0;
  int length = xid.gtrid_length + xid.bqual_length;
  for(int i=0; i<length; ++i) {
    char ch = ((xid.data[i] < 26) ? '*' : xid.data[i]);
    out /*<< (int)xid.data[i] << ","*/ << ch /* << ":"*/;
  }
  out << "(" << xid.gtrid_length << "," << xid.bqual_length << ")";
}
#endif

void UAKGQueryUtils::printFieldDescription(ostream& out, 
                                           const FieldDescription& fd)
{
 out << fd.name << ':';
 switch(fd.type)
 {
  case TypeBoolean:
        out << "Boolean" ;
        break;
  case TypeChar:
        out << "Char" ;
        break;
  case TypeOctet:
        out << "Octet" ;
        break;
  case TypeShort:
        out << "Short" ;
        break;
  case TypeUShort:
        out << "UShort" ;
        break;
  case TypeLong:
        out << "Long" ;
        break;
  case TypeULong:
        out << "ULong" ;
        break;
  case TypeLongLong:
        out << "LongLong" ;
        break;
  case TypeULongLong:
        out << "ULongLong" ;
        break;
  case TypeFloat:
        out << "Float" ;
        break;
  case TypeDouble:
        out << "Double" ;
        break;
  case TypeString:
        out << "String" ;
        break;
  case TypeSmallInt:
        out << "SmallInt" ;
        break;
  case TypeInteger:
        out << "Integer" ;
        break;
  case TypeReal:
        out << "Real" ;
        break;
  case TypeDoublePrecision:
        out << "DoublePrecision" ;
        break;
  case TypeCharacter:
        out << "Character" ;
        break;
  case TypeDecimal:
        out << "Decimal" ;
        break;
  case TypeDateTime:
        out << "DateTime" ;
        break;
  case TypeNumeric:
        out << "Numeric" ;
        break;
  case TypeRaw:
        out << "Row" ;
        break;
  case TypeBlob:
        out << "Blob" ;
        break;
  case TypeClob:
        out << "Clob" ;
        break;
  case TypeWString:
        out << "WString" ;
        break;
  case TypeWclob:
        out << "Wclob" ;
        break;
  default:
        out << "UnknownFieldType" ;
        break;
 }
}

void UAKGQueryUtils::printFieldType(ostream& out, FieldType ft)
{
 switch(ft)
 {
  case TypeBoolean:
        out << "Boolean" ;
        break;
  case TypeChar:
        out << "Char" ;
        break;
  case TypeOctet:
        out << "Octet" ;
        break;
  case TypeShort:
        out << "Short" ;
        break;
  case TypeUShort:
        out << "UShort" ;
        break;
  case TypeLong:
        out << "Long" ;
        break;
  case TypeLongLong:
        out << "LongLong" ;
        break;
  case TypeULong:
        out << "ULong" ;
        break;
  case TypeULongLong:
        out << "ULongLong" ;
        break;
  case TypeFloat:
        out << "Float" ;
        break;
  case TypeDouble:
        out << "Double" ;
        break;
  case TypeString:
        out << "String" ;
        break;
  case TypeSmallInt:
        out << "SmallInt" ;
        break;
  case TypeInteger:
        out << "Integer" ;
        break;
  case TypeReal:
        out << "Real" ;
        break;
  case TypeDoublePrecision:
        out << "DoublePrecision" ;
        break;
  case TypeCharacter:
        out << "Character" ;
        break;
  case TypeDecimal:
        out << "Decimal" ;
        break;
  case TypeDateTime:
        out << "DateTime" ;
        break;
  case TypeNumeric:
        out << "Numeric" ;
        break;
  case TypeRaw:
        out << "Row" ;
        break;
  case TypeBlob:
        out << "Blob" ;
        break;
  case TypeClob:
        out << "Clob" ;
        break;
  case TypeWString:
        out << "WString" ;
        break;
  case TypeWclob:
        out << "Wclob" ;
        break;
  default:
        out << "UnknownFieldType" ;
        break;
 }
}


void UAKGQueryUtils::printRecordSet(ostream& out, RecordSet* rs)
{
 
 for(CORBA::ULong row=0; row<rs->getNRows(); ++row) {
   out << '|';
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
	    case TypeLongLong:
	           out << rs->getLongLongAt(row,col);
	           break;
	    case TypeULongLong:
	           out << rs->getULongLongAt(row,col);
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
	        {
	           Blob_var bl = rs->getBlobAt(row,col);
	           CORBA::String_var str =
	              UAKGQuerySingleton::getORB_ptr()->object_to_string(bl);
           	   out << "Blob_ior: " << str.in();
          	}
          	break;
	    case TypeClob:
	          {
	           Clob_var cl = rs->getClobAt(row,col);
	           CORBA::String_var str =
                         UAKGQuerySingleton::getORB_ptr()->object_to_string(cl);
       		    out << "Clob_ior: " << str.in();
	          }
	          break;
	    case TypeWString:
	          {
	           CORBA::String_var s = (const char*) 
				rs->getWStringAt(row,col);
	           out << s;
	          }
	          break;
	    case TypeWclob:
	          {
	           Wclob_var wcl = rs->getWclobAt(row,col);
	           CORBA::String_var str =
	               UAKGQuerySingleton::getORB_ptr()->object_to_string(wcl);
        	   out << "Wclob_ior: " << str.in();
	          }
	          break;
	    default:
	          out << "UnknownFieldType : <" << rs->getFieldTypeAt(col) << ">";
        } // switch
        out << '|';
   }     
   out << endl;
 }
}

void UAKGQueryUtils::printRecordSetDescription(ostream& out, RecordSet* rs)
{
 for(CORBA::ULong col=0; col<rs->getNColumns(); ++col) {
   CORBA::String_var s = rs->getColumnNameByIndex(col);
   out << s.in() << ":";
   printFieldType(out,rs->getFieldTypeAt(col));
   if (col!=rs->getNColumns()-1) out << "," ;
 }
}


void UAKGQueryUtils::printNumeric(ostream& out, const Numeric* num)
{
  Numeric* ptr = const_cast<Numeric*>(num);
  // CORBA::String_var s = CORBA::string_dup( ptr->getAsString() );
  CORBA::Double s =  ptr->getAsDouble();
  out << s;
}

void UAKGQueryUtils::printDateTime(ostream& out, const DateTime& dt)
{
  out << setw(2) << (int)dt.day << ':' <<
                     (int)dt.month << ':' <<
         setw(4) << (int)dt.year << ':' <<
         setw(2) << (int)dt.hour << ':' <<
                     (int)dt.minute << ':' <<
                     (int)dt.second; 
}

void UAKGQueryUtils::printRaw(ostream& out,const OctSeq* octSeq)
{
 for(CORBA::ULong i=0; i<octSeq->length(); ++i){
   int x=(int)((*octSeq)[i]);
   out << setw(2) << '[' << x << ']' ;
 }
}

bool UAKGQueryUtils::checkDate ( const UAKGQuery2::DateTime& dt )
{
    // Corresponding to the Grigorian calendar
    return (
        dt.day>0 && dt.month>0 && dt.month<13 && (
            dt.day<29 || (
                dt.day<31 && (
                    dt.month!=2 || (
                       dt.day<30 && (
                           (dt.year%4)==0 && (
                               (dt.year%100)!=0 || (dt.year%400)==0
                           )
                       )
                    )
                )
            ) || (
               dt.day<32 && !(
                   dt.month==4 || dt.month==6 || dt.month==9 || dt.month== 11 
|| dt.month==2
               )
            )
        ) && dt.hour<24 && dt.minute<60 &&  dt.second<60
    );
}

std::string UAKGQueryUtils::longToString(long x)
{
 std::ostringstream ostr;
 ostr << x;
 return ostr.str();
}


/**
 * code s with method 'methodCode' using key 'key'
 *  in such way, that
 *      uncode(code(s,ch,key,m),ch,key,m)=s for any s.
 *
 *  mehtod 0:
 *      assumed, that 
 *           's' consists from ASCII symbols. 
 *           coding is not crypto-strong
 *      additional warranty, that:     
 *             return string does not contains character expCh.
 *             'code(s)' consists from symbols. [ 20 - 255]
 *
 *  method other:
 *          not implemented.
 *
 **/
std::string UAKGQueryUtils::code(const std::string& s, char expCh, 
		                 const std::string& key, 
				 int methodCode)
{
 // method code now ignored.
 std::string retval(s);
 int slen=s.length();
 int klen=key.length();
 int si=0;
 int ki=0; // index of key
 for(si=0; si<slen; ++si) {
   char ch= (s[si] ^ key[ki] );
   ch+=20;  
   if (ch >= expCh ) ++ch;
   ki=((ki+1)%klen);
   retval[si]=ch;
 }
 return retval;
}
		           
/**
 * uncode s, previously coded by code with same arguments
 **/
std::string UAKGQueryUtils::uncode(const std::string& s, char expCh, 
		            const std::string& key, int methodCode)
{
 int slen=s.length();
 int klen=key.length();
 int si=0;
 int ki=0; // index of key
 std::string retval(s);
 for(si=0; si<slen; ++si) {
    char ch=s[si];
    if (ch>expCh) --ch;
    ch-=20;
    ch=(ch^key[ki]);
    ki=((ki+1)%klen);
    retval[si]=ch;
 }
 return retval;
}

