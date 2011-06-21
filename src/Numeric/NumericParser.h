#ifndef __Numeric_NumericParser_h
#define __Numeric_NumericParser_h

#include <stdlib.h>
// for NULL

class NumericParser
{
private: 

 char sign_;
 const char* beforeDotDigits_;
 int         nBeforeDotDigits_;
 const char* afterDotDigits_;
 int         nAfterDotDigits_;
 const char* expDigits_;
 int  exponente_;
 char expSign_;
 
 int nLeading0s_;

public:

 NumericParser()
  :sign_('+'),
   beforeDotDigits_(NULL),
   nBeforeDotDigits_(0),
   afterDotDigits_(NULL),
   nAfterDotDigits_(0),
   expDigits_(NULL),
   exponente_(0),
   expSign_('+'),
   nLeading0s_(0)
 {}

 char  getSign() const { return sign_; } 
 const char* getBeforeDotDigits() const { return beforeDotDigits_; }
 int         getNBeforeDotDigits() const { return nBeforeDotDigits_; }
 const char* getAfterDotDigits() const { return afterDotDigits_; }
 int         getNAfterDotDigits() const { return nAfterDotDigits_; }
 const char* getExpDigits() const { return expDigits_; }
 int   getExponente() const { return exponente_; }
 char  getExpSign() const { return expSign_; } 


 int   getNLeading0s() { return nLeading0s_; } 


 //
 // read:
 //

 char readSign(const char*& sz);

 const char* readDigits(const char*& sz, int skip0s, int& ndigits);

 void parse(const char* sz);

 void trim(const char*& sz);

};

#endif
