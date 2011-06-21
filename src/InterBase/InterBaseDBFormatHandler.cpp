#include <InterBase/InterBaseDBFormatHandler.h>

#include <iostream>
using namespace std;
using namespace UAKGQuery2;

#include <time.h>
#include <math.h>

#define MAX_MANTISSA_SIZE 5
#define MAX_DECIMAL_SIZE 8
             // 1 for exp, 1 for terminator

typedef unsigned char ub1;
typedef signed char sb1;

InterBaseDBFormatHandler interBaseDBFormatHandler;


void* InterBaseDBFormatHandler::decimal2rawmem(Numeric* dec,
                                          const FieldDescription& fd)
{
  NumericImpl* dw = dynamic_cast<NumericImpl*>(dec);
  CORBA::Double d = dw->getAsDouble();
  ISC_INT64* retval = new ISC_INT64();
  // TODO: rewrite without usage of double
  *retval = d*pow(10, abs(fd.scale));
  return retval;
}

Numeric* InterBaseDBFormatHandler::rawmem2decimal(void* x, const FieldDescription& fd)
{
  ISC_INT64 y = (ISC_INT64) *(ISC_INT64 ISC_FAR *)x;
  double d = y / (pow(10, fd.scale));
  Numeric_var num = new NumericImpl();
  num->setAsDouble(d);
  return num._retn();
}


/*
 *
 *  Oracle Call Interface Programmer's Guide, Volumes 1 & 2
 *   Release 8.0
 *   A58234-01
 *
 * Oracle stores values of the NUMBER datatype in a variable-length format. 
 * The first byte is the exponent and is followed by 1 to 20 mantissa bytes. 
 * The high-order bit of the exponent byte is the sign bit; it is set for positive numbers. 
 * The lower 7 bits represent the exponent, which is a base-100 digit with an offset of 65.
 * 
 * Each mantissa byte is a base-100 digit, in the range 1..100. For positive numbers,
 * the digit has 1 added to it. So, the mantissa digit for the value 5 is 6. For negative numbers,
 * instead of adding 1, the digit is subtracted from 101. So, the mantissa digit for the number -5 is 96 (101-5). 
 * Negative numbers have a byte containing 102 appended to the data bytes.
 * However, negative numbers that have 20 mantissa bytes do not have the trailing 102 byte. 
 * Because the mantissa digits are stored in base 100, each byte can represent 2
 * decimal digits. The mantissa is normalized; leading zeroes are not stored. 
 * 
 * Up to 20 data bytes can represent the mantissa. However, only 19 are guaranteed to be accurate. 
 * The 19 data bytes, each representing a base-100 digit, yield a maximum
 * precision of 38 digits for an Oracle NUMBER. 
 * 
 * 
 * 
 *
 * VARNUM
 *
 * The VARNUM datatype is like the external NUMBER datatype, except that the first 
 * byte contains the length of the number representation. This length does not 
 * include the length byte itself. Reserve 22 bytes to receive the longest possible VARNUM. 
 * Set the length byte when you send a VARNUM value to Oracle. 
 * 
 * Table 3 - 3 shows several examples of the VARNUM values returned for numbers in an Oracle table. 
 * 
 * Table 3-3 VARNUM Examples 
 * DecimalValue | LengthByte | ExponentByte | MantissaBytes | TerminatorByte |
 *      0       |  1         |     128      |      n/a      |    n/a         |
 *      5       |  2         |     163      |       6       |    n/a         |
 *     -5       |  3         |     62       |      96       |      102       |
 *    2767      |  3         |     194      |   28, 68      |     n/a        |
 *   -2767      |  4         |     61       |  74, 34       |    102         |
 *  100000      |  2         |  195         |   11          |    n/a         |
 *  1234567     |   5        |   196        |  2,24,46,68   |    n/a         |
 *
 */

/*--------------------------------------------------------------------*/
/*******
 * work with DATE type (DateTime):
 **/

void* InterBaseDBFormatHandler::datetime2rawmem(const DateTime& datetime,
                                          const FieldDescription& fd)
{
 tm tm_date;
 tm_date.tm_year = datetime.year - 1900;
 tm_date.tm_mon = datetime.month -1;
 tm_date.tm_yday = datetime.day - 1;
 tm_date.tm_hour = datetime.hour;
 tm_date.tm_min = datetime.minute;
 tm_date.tm_sec = datetime.second;
 int i;
 for (i=1; i<datetime.month; i++)
 {
    switch(i)
    {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
         tm_date.tm_yday += 31;
         break;
      case 2:
         if (tm_date.tm_year % 4 == 0)
         {
           tm_date.tm_yday += 29;
         }else{
           tm_date.tm_yday += 28;
         }
         break;
      case 4:
      case 6:
      case 9:
      case 11:
         tm_date.tm_yday += 30;
         break;
    }
 }

 ISC_QUAD* retval = new ISC_QUAD;

 isc_encode_date(&tm_date, retval);

 return retval;
}

DateTime InterBaseDBFormatHandler::rawmem2datetime(void* data,
                                          const FieldDescription& fd)
{
 tm tm_date;
 isc_decode_date((ISC_QUAD*) data, &tm_date);
 DateTime retval ;
 retval.year = tm_date.tm_year + 1900;
 retval.month = tm_date.tm_mon + 1;
 retval.day = tm_date.tm_yday + 1;
 retval.hour = tm_date.tm_hour;
 retval.minute = tm_date.tm_min;
 retval.second = tm_date.tm_sec;
 int i;
 for (i=1; i<retval.month; i++)
 {
    switch(i)
    {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
         retval.day -= 31;
         break;
      case 2:
         if (retval.year % 4 == 0)
         {
           retval.day -= 29;
         }else{
           retval.day -= 28;
         }
         break;
      case 4:
      case 6:
      case 9:
      case 11:
         retval.day -= 30;
         break;
    }
 }
 return retval;
}


/**
 * for Date:
 * (tabe 3-4) the DATE Datatype 
 * 
 *  Byte     Meanimg
 *    1       Century
 *    2       Year
 *    3       Month
 *    4       Day
 *    5       Hour
 *    6       Minute
 *    7       Second
 *
 * Century and year bytes are in an excess-100 notation. 
 * Dates Before Common Era (BCE) are less than 100. 
 * The era begins on 01-JAN-4712 BCE, which is Julian day 1. For
 * this date, the century byte is 53, and the year byte is 88. 
 * The hour, minute, and second bytes are in excess-1 notation.
 * The hour byte ranges from 1 to 24, the minute and second
 * bytes from 1 to 60. If no time was specified when the date was created,
 * the time defaults to midnight (1, 1, 1). 
 *
 * When you enter a date in binary format using the DATE external datatype, 
 * the database does not do consistency or range checking. 
 * All data in this format must be carefully
 *  validated before input. 
 **/

//
// Object:
//

/* nothing there, all defined up */
/* for FUTURE: add handling of Oracle Objects via Oracle POA */

//
// Raw
//

/**
 * transform Raw field to raw internal representation, which is
 * siutable for using in bind variables.
 *
 */
void InterBaseDBFormatHandler::raw2rawmem_fill(void* buffer, CORBA::ULong bufflen,
                                 void* memptr,
                                 const FieldDescription& fd )
{
	ISC_QUAD* tmp = (ISC_QUAD*)memptr;
	const CORBA::Octet* cur = (CORBA::Octet*)buffer;
	(*tmp).gds_quad_high = cur[0] + (cur[1]<<8) + (cur[2]<<16) + (cur[3]<<24);
	(*tmp).gds_quad_low = cur[4] + (cur[5]<<8) + (cur[6]<<16) + (cur[7]<<24);
}

/**
 * transform internal raw memory representation into raw field.
 *
 * memory for return value is allocated by CORBA allocators
 **/
OctSeq*  InterBaseDBFormatHandler::rawmem2raw(void* mp,
                                            const FieldDescription& fd)
{
 ISC_QUAD* tmp = (ISC_QUAD*)mp;
 long high = (*tmp).gds_quad_high;
 long low = (*tmp).gds_quad_low;
 OctSeq_var retval;
 retval->length(8);
 retval[0] = (high &0xff);
 retval[1] = (high >> 8);
 retval[2] = (high >> 16);
 retval[3] = (high >> 24);
 retval[4] = (low &0xff);
 retval[5] = (low >> 8); 
 retval[6] = (low >> 16);
 retval[7] = (low >> 24);
 return retval._retn();
}

/**
 * return length of packet represntation of raw
 **/
CORBA::ULong    InterBaseDBFormatHandler::raw_length(const FieldDescription& fd)
{
//  return fd.size+2;
   return sizeof(ISC_QUAD);
}


//TODO:
// insert documentation here.

