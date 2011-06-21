#include <Oracle8/Oracle8DBFormatHandler.h>
#include <Oracle8/Oracle8Query.h>

#include <iostream>
using namespace std;
using namespace UAKGQuery2;

#define MAX_MANTISSA_SIZE 19
#define MAX_DECIMAL_SIZE 22
             // 1 for exp, 1 for terminator


void* Oracle8DBFormatHandler::decimal2rawmem(Numeric* dec,
                                          const FieldDescription& fd)
{
  NumericImpl* dw = dynamic_cast<NumericImpl*>(dec);
  if (dw==NULL) {
    throw QueryProcessingError("Impossible: can't narrow valuetype to implementation",501,0);
  }
 
  ub1* retval = new ub1[MAX_DECIMAL_SIZE]; 
  memset(retval,0,MAX_DECIMAL_SIZE);

  if (dw->is0()) { 
    retval[1]=0x80;
    retval[0]=1;
    return retval;
  }

  int exponente10 = dw->exponente();

  int leading0, trailing0;
  if (dw->exponente()%2==0 && dw->nDigits()%2==0) {
    leading0=0; trailing0=0; 
  }else if(dw->exponente()%2==0 && dw->nDigits()%2==1) {
    leading0=0; trailing0=1; 
  }else if(dw->exponente()%2==1 && dw->nDigits()%2==0) {
    leading0=1; trailing0=1; 
  }else{ //dw->exponente()%2==1 && dw->nDigits()%2==1
    leading0=1; trailing0=0; 
  }

  ub1 exponente100=dw->exponente();
  if (leading0) {
     ++exponente100;
  }
  exponente100/=2;

  //write exponente
  retval[1]=(dw->positive() ? (exponente100+64)|0x80:((63-exponente100)&0x7F));

  ub1* rcur = retval+2;
  int nDigit = 0;
  if (leading0) {
    int digit = dw->readDigit(nDigit++);
    *rcur++=((dw->positive()) ? digit+1 : (101-digit));
  }
  while(nDigit < dw->nDigits() && nDigit < MAX_MANTISSA_SIZE)
  {
    int d0=dw->readDigit(nDigit++);
    int d1;
    if (nDigit==dw->nDigits()) {
      if (!trailing0) {
         cerr << "! internal error" << __FILE__ <<","<< __LINE__ << endl;
      }
      d1=0;
    }else{
      d1=dw->readDigit(nDigit++);
    }
    int handre=d0*10+d1;
    *rcur++ = ((dw->positive()) ? (handre+1) : (101-handre));
  }
  if (dw->nDigits()<38) {
   if (dw->positive()) {
     // do nothing 
   }else{
    *rcur++=102;  // terminator 
   }
  }
  retval[0]=rcur-retval-1;

  return retval;
}



Numeric* Oracle8DBFormatHandler::rawmem2decimal(void* x, const FieldDescription& fd)
{
  NumericImpl* dw = new NumericImpl;

  ub1* xx = (ub1*)(x);

  ++xx;
  sb1 exponente100 = *xx++; 
  if (exponente100==(sb1)0x80) {
    return dw;
  }

  int positive = ((exponente100 & (sb1)0x80) != 0);
  exponente100 &= 0x7F; // 7F=01111111
  exponente100 = ((positive) ? exponente100-65+1 : 128-65-exponente100);

  int len=*((ub1*)x);

  // write 1-st byte
  //  we do not want to normalize digit after writing, so better
  //  transform normalized base-100 to normalized BCD, by
  //  skipping leading 0 and adding 1 to exponente.
  int handre=(positive?(*xx-1):(101-*xx));
  int exp_correction=0; 

  if (handre < 10) {
    dw->appendDigit(handre);
    exp_correction=1;
  }else{
    dw->appendHandre(handre);
  }
  ++xx;
  for(int i=0;i<len;++i,++xx){
     if (*xx < 1 || *xx > 100) {
        break;
     }
     int handre=(positive?(*xx-1):(101-(*xx)));
     dw->appendHandre(handre);
  }

  dw->_exponente(exponente100*2-exp_correction);
  dw->_positive(positive);

  dw->normalizeRight();
  CORBA::String_var debug_sz = dw->createStringExp();

  return dw;
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

void* Oracle8DBFormatHandler::datetime2rawmem(const DateTime& datetime,
                                         const FieldDescription& fd)
{
 unsigned char* retval = new unsigned char[7];
 retval[0]= (datetime.year/100)+100;
 retval[1]= (datetime.year%100)+100;
 retval[2]= datetime.month;
 retval[3]= datetime.day;
 retval[4]= (datetime.hour+1)%25;
 retval[5]= (datetime.minute+1)%61;
 retval[6]= (datetime.second+1)%61;
 return retval;
}

DateTime Oracle8DBFormatHandler::rawmem2datetime(void* data,
                                          const FieldDescription& fd)
{
 unsigned char* raw= (unsigned char*)data;
 DateTime retval ;
 retval.year = (raw[0]-100)*100 + (raw[1]-100);
 retval.month = raw[2];
 retval.day = raw[3];
 retval.hour = raw[4]-1;
 retval.minute = raw[5]-1;
 retval.second = raw[6]-1;
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
void Oracle8DBFormatHandler::raw2rawmem_fill(void* buffer, CORBA::ULong bufflen, 
                                 void* memptr,
                                 const FieldDescription& fd )
{
  ub1* mptr = (ub1*)memptr;  
  ub1* x = (ub1*)buffer;  
  ub2* mplen = (ub2*)mptr;
  *mplen = (ub2)bufflen;
  mptr+=2; 
  memcpy(mptr,x,bufflen);
}

/**
 * transform internal raw memory representation into raw field.
 * 
 * memory for return value is allocated by CORBA allocators 
 **/
OctSeq*  Oracle8DBFormatHandler::rawmem2raw(void* mp, 
                                            const FieldDescription& fd)
{
 ub1* mptr = (ub1*)mp;
 ub2 len = *((ub2*)mp);
 mptr+=2;
 OctSeq_var retval = new OctSeq(len,len,mptr,0);
 return retval._retn();
}

/**
 * return length of packet represntation of raw
 **/
CORBA::ULong    Oracle8DBFormatHandler::raw_length(const FieldDescription& fd)
{
  return fd.size+2;
}

void* Oracle8DBFormatHandler::allocate(FieldType t, size_t size) {
    //char* p = new char[size]; // check here
    char* p = new char[size];
    memset(p,0,size*sizeof(char));
    switch (t) {
        case TypeBlob:
        case TypeClob:
        case TypeWclob:
            {
                OCILobLocator** ll = reinterpret_cast<OCILobLocator**>(p);
                OCILobLocator** ll_end =
                        reinterpret_cast<OCILobLocator**>(p + size);
        //for(;ll < ll_end;ll+=sizeof(OCILobLocator*)) { //check here
                for(;ll < ll_end;++ll) {
                    q_->checkError(
                        OCIDescriptorAlloc(
                                q_->getOCIEnv_p(),
                                (dvoid**)ll,
                                OCI_DTYPE_LOB,
                                (size_t)0,
                                (dvoid**)NULL)
                    );
                }
            }
    }
    return p;
}

void Oracle8DBFormatHandler::deallocate(FieldType t, void*& vpa, size_t size)
{
    void* vp=vpa;
    char* p = reinterpret_cast<char*>(vp);
    bool wasError=false;
    switch (t) {
        case TypeBlob:
        case TypeClob:
        case TypeWclob:
            {

                OCILobLocator** ll = reinterpret_cast<OCILobLocator**>(p);
                //std::cerr << "deallocate, ll=" << ll << std::endl;
                OCILobLocator** ll_end =
                        reinterpret_cast<OCILobLocator**>(p + size);
                for(;ll < ll_end;++ll) {
                 //std::cerr << "*ll=" << *ll << std::endl;
                  try {
                    q_->checkError(
                       OCIDescriptorFree((dvoid*)(*ll), (ub4) OCI_DTYPE_LOB)
                    );
                  } catch (const UAKGQuery2::QueryProcessingError& ex) {
                    wasError=true;
                    q_->getLogger().warnings() << "Error during Oracle LOB deallocation:" << ex.why << std::endl; 
                  }
                }
            }
    }
    delete [] reinterpret_cast<char*>(vp);
    vpa = NULL;
}

//TODO:
// insert documentation here.

