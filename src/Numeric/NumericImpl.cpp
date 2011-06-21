#include <Numeric/NumericImpl.h>
#include <UAKGQuerySingleton.h>
#include <ctype.h>
#include <errno.h>

using namespace std;

#define get_first_digit_in_byte(byte)  (((byte)&0xF0)>>4)
#define get_second_digit_in_byte(byte)  ((byte)&0x0F)
#define get_last_digit_in_byte(byte)  ((byte)&0x0F)

#define set_first_digit_in_new_byte(byte,digit)  (byte=(((digit)<<4)|0x0F))
#define set_last_digit_in_new_byte(byte,digit)  (byte&=((digit)|0xF0))
#define set_first_digit_in_byte(byte,digit)  ((byte&=0x0F)|=((digit)<<4))
#define set_second_digit_in_byte(byte,digit)  ((byte&=0xF0)|=(digit))
#define set_last_digit_in_byte(byte,digit)  ((byte&=0xF0)|=(digit))

#define set_handre_in_byte(byte,handre)  (byte=((handre)/10<<4)+((handre)%10))

#define byte_from_digit(nDigit)      ((nDigit)/2+2)

namespace UAKGQuery2 {

void NumericImpl::init()
{ nDigits_=0;
  precision(0);
  scale(0);
  value().length(2);
  value()[0]=0;
  value()[1]='+';
}

void NumericImpl::initnDigits()
{
 int dLength = value().length();
 if (dLength<=2 ) {
    nDigits_=0;
 }else{
    nDigits_=(dLength-2)*2-(get_last_digit_in_byte(value()[dLength-1]) > 9);
 }
}

int NumericImpl::is0(void) const 
{ return nDigits_==0; }

CORBA::ValueBase* NumericImpl::_copy_value()
{
   NumericImpl*  num = new NumericImpl;

   num->value(value());
   num->precision(precision());
   num->scale(scale());
   num->value(value());

   return num;
}

char* NumericImpl::getAsString()
{
  CORBA::String_var s = createStringExp();
  return s._retn();
}

CORBA::Long NumericImpl::getAsLong()
{
 CORBA::String_var s = createStringExp();
 return (CORBA::Long) getAsDouble();
}

CORBA::Double NumericImpl::getAsDouble()
{
 CORBA::String_var s = createStringExp();
 errno = 0;
 char * end;
 double d = strtod(s.in(), &end);
 if(errno == ERANGE) {
    // 1 - code of overflow error
    UAKGQuerySingleton::getLogger().errors() << 
          "InvalidConversion: strtod overflow for  " << s.in() << std::endl;
    throw InvalidConversion(1);
 }
 if(errno||(!strcmp(s,end))) {
    UAKGQuerySingleton::getLogger().errors() << 
          "InvalidConversion: strtod failed for " << s.in() << std::endl;
    // 2 - code of conversation error
    throw InvalidConversion(2);
 }
 return d;
}

void NumericImpl::setAsString(const char* str)
{
  parse(str);
  CORBA::String_var debug_str = createStringExp();
}

void NumericImpl::setAsLong(CORBA::Long l)
{
    // TODO : implement more simple algorith of setting long
    // Temporary setAsLong implemented as setAsDouble because it is approved.
    setAsDouble(l);
}

void NumericImpl::setAsDouble(CORBA::Double d)
{
 char buff[40];
 sprintf(buff,"%f",d);
 parse(buff);
}


char* NumericImpl::createStringExp() const
{
  int sLen=stringExpLen();
  char* retval=CORBA::string_alloc(sLen);
  fillStringExp(retval);
  return retval;
}

int NumericImpl::stringExpLen() const
{
 return 2+nDigits()+1+4+1;
        //2([-].) +nDigits (xxxx) 1('e') + 4 (exp) +1 ('\0')
}

void  NumericImpl::fillStringExp(char* sz) const
{
  if (negative())  *sz++='-';
  *sz++='.';
  int nDigit=0;
  for(; nDigit<nDigits(); ++nDigit){
    *sz++=readDigit(nDigit)+'0';
  }
  if (nDigit==0) *sz++='0';
  *sz++='e';
  sprintf(sz,"%d",exponente());
}

char* NumericImpl::createStringFixed() const
{
 char* retval=CORBA::string_alloc(2+precision());
 fillStringFixed(retval);
 return retval;
}

void NumericImpl::fillStringFixed(char* sz) const
{
 int lprecision=precision();
 int lscale=scale();
 int lexponente=exponente();

 if (lexponente <= 0) {
   *sz++='0';
   *sz++='.';
   while(lexponente<0) {
     *sz++='0';
     ++lexponente;
   }
   for(int i=0;i<nDigits();++i) {
      *sz++=readDigit(i)+'0';
   }
 }else{
   for(int i=0;i<nDigits();++i) {
     if(lexponente==0) {
        *sz++='.';
     }
     --lexponente;
     *sz++=readDigit(i)+'0';
   }
 }
 *sz++='\0';
}


/**
 * resize internal decimal, to allow at least nBytes+1 be present
 * (i. e. v_->value[nBytes] after resizing is correct.
 **/
void NumericImpl::resize(int nBytes)
{
 if (++nBytes >= value().length()) {
    value().length(nBytes);
 }
}

int NumericImpl::readDigit(int nDigit) const
{
 int nByte = byte_from_digit(nDigit);
 //assert(nByte<value().length());
 int retval;
 if ((nDigit%2)==0)
    retval = get_first_digit_in_byte(value()[nByte]);
 else
    retval=get_second_digit_in_byte(value()[nByte]);
 return retval;
}


void NumericImpl::_writeDigit(int digit, int nDigit)
{
 int nByte=byte_from_digit(nDigit);
 if ((nDigit % 2)==0) {
   unsigned char b = value()[nByte];
   value()[nByte] = ( (b&=0x0F) |= ((unsigned char)digit<<4) );
 }else{
   unsigned char b = value()[nByte];
   value()[nByte] = ( (b&=0xF0) |= ((unsigned char)digit) );
 }
}

/**
 * normilize left part, by stripping leading 0-s and
 * correction of exponente.
 * i. e. .00033e5 = .33e2
 *       .009e-1 = .9e-4
 *       .009e1 = .9e-2
 *       .000001e3 = .00001e2 = .0001e1 = .001e-1 = .01e-2=.1e-3
 *       .013e3=.13e2
 **/
void NumericImpl::normalizeLeft()
{
 if (nDigits()==0) return;
 int nLeadingNulls=0;
 int exponente10=exponente();
 while(nLeadingNulls < nDigits_ && (readDigit(nLeadingNulls) == 0)) {
       ++nLeadingNulls;
       --exponente10;
 }
 _exponente(exponente10);

 //shiftLeft(nLeadingNulls);
 // :
 if (nLeadingNulls!=0) {

   for(int i=nLeadingNulls;i<nDigits();++i){
     _writeDigit(readDigit(i),i-nLeadingNulls);
   }
   nDigits_-=nLeadingNulls;
   precision(precision()-nLeadingNulls);
   scale(scale()-nLeadingNulls);
   if (nDigits_!=0) {
     value().length(byte_from_digit(nDigits_-1)+1);
   }else{
     value().length(2);
   }
 }
}

/**
 * set scale and precision of digit by exponente
 **/
void NumericImpl::normalizeExponente()
{
 if (nDigits_ == 0) {
   scale(0);
   precision(0);
   _exponente(0);
 }else{
   signed char& exp = reinterpret_cast<signed char&>(value()[0]);
   if (exp < 0) {
     scale(nDigits_-exp);
     precision(nDigits_-exp);
   }else if (exp > 0) {
     if (nDigits_ > exp) {
      scale(nDigits_-exp);
      precision(nDigits_);
     }else{
      scale(0);
      precision(nDigits_-exp);
     }
   }else{
     scale(nDigits_);
     precision(nDigits_);
   }
 }
}

/**
 * normalize right part by elimination of trailing 0-s.
 * i.e.
 *    .330 e 3 = .33e3
 *    .500 e -2 = .5 e -2
 **/
void NumericImpl::normalizeRight()
{
 int nTrailingNulls=0;
 for(int nDigit = nDigits()-1; nDigit>=0; --nDigit) {
   if(readDigit(nDigit)!=0) break;
   else{
     _writeDigit(0x0F,nDigit); // set 1111
     ++nTrailingNulls;
   }
 }
 if (nTrailingNulls!=0) {
   nDigits_-=nTrailingNulls;
   precision(precision()-nTrailingNulls);
   scale(scale()-nTrailingNulls);
   if (nDigits_!=0) {
     value().length(byte_from_digit(nDigits_-1)+1);
   }else{
     value().length(2);
   }
 }
}

void  NumericImpl::normalize()
{
 normalizeLeft();
 normalizeRight();
 normalizeExponente();
}


void NumericImpl::parse(const char* sz)
{
 NumericParser parser;
 parser.parse(sz);
 const char* cur=parser.getBeforeDotDigits();
 if (cur==NULL) {
   appendDigit(0);
 }
 for(;;cur++) {
  if (isdigit(*cur)) appendDigit(*cur-'0');
  else if (*cur=='.') continue;
  else break;
 }
 int exponente=(int)parser.getExponente()+parser.getNBeforeDotDigits();
 _exponente(exponente);
 _sign(parser.getSign());
 normalize();
}

void NumericImpl::appendHandre(int handre)
{
 int nByte=byte_from_digit(nDigits_+1);
 resize(nByte);
 if ((nDigits_%2)==0) {
    set_handre_in_byte(value()[nByte],handre);
 }else{
    set_last_digit_in_new_byte(value()[nByte-1],handre/10);
    set_first_digit_in_new_byte(value()[nByte],handre%10);
 }
 nDigits_+=2;
 scale(scale()+2);
 precision(precision()+2);
}


void NumericImpl::appendDigit(int digit)
{
 int nByte=byte_from_digit(nDigits_);
 resize(nByte);
 if ((nDigits_%2)==0) {
    set_first_digit_in_new_byte(value()[nByte],digit);
 }else{
    set_last_digit_in_new_byte(value()[nByte],digit);
 }
 ++nDigits_;
 scale(scale()+1);
 precision(precision()+1);
}

NumericImpl::~NumericImpl()
{
}


NumericFactoryImpl::NumericFactoryImpl()
{
}

NumericFactoryImpl::~NumericFactoryImpl()
{
}

CORBA::ValueBase*
NumericFactoryImpl::create_for_unmarshal()
{
    return new NumericImpl;
}

Numeric*
NumericFactoryImpl::create()
{
    return new NumericImpl;
}

}

