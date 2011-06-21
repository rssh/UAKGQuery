/*
 * part of Grad-Soft UAKGQuery2.
 * (C) Grad-Soft Ltd, Kiev, Ukraine.
 * $Id: NumericParser.cpp,v 1.3 2002-10-23 15:23:11 srkr Exp $
 */

#include <Numeric/NumericParser.h>
#ifndef __importUAKGQuery2_h
#include <importUAKGQuery2.h>
#endif

char NumericParser::readSign(const char*& sz)
{
 if (*sz=='-' || *sz=='+') 
     return *sz++;
 else
     return '+';
}

void NumericParser::trim(const char*& sz)
{
    /*
    *  Trim spaces and tabs before expression
    */
    while(sz[0] != 0 && (sz[0] == '\t' || sz[0] == ' ')){
        sz++;
    }
}

const char* NumericParser::readDigits(const char*& sz, int skip0, int& ndigits)
{
 int end_digits=0;
 ndigits=0;
 const char* retval;
 retval=sz;
 if (skip0) {
   while(*sz!=0 && *sz=='0') {
      ++sz;
      ++ndigits;
      ++nLeading0s_;
   }
 }
 while(!end_digits){
   switch(*sz) {
     case  '0':
     case  '1':
     case  '2':
     case  '3':
     case  '4':
     case  '5':
     case  '6':
     case  '7':
     case  '8':
     case  '9':
              ++sz;
              ++ndigits;
              break;
     default:
              end_digits=1;
              break;
   }
 }
 return retval;
}

enum ParseState {
 stBEGIN,stBEFORE_DIGITS,stAFTER_DOT,stBEFORE_EXPONENTE,stEND
};

void NumericParser::parse(const char* sz)
{
 const char* dotPointer=NULL; 
 int nExpDigits=0;

 ParseState state=stBEGIN;
 while(state!=stEND)
 {
   switch(state) {
     case stBEGIN: trim(sz);
                   sign_=readSign(sz);
                   state=stBEFORE_DIGITS;
                   break; 
     case stBEFORE_DIGITS:
                   beforeDotDigits_=readDigits(sz,1,nBeforeDotDigits_); 
                   switch(*sz) {
                     case '.': 
                             dotPointer=sz++;
                             state=stAFTER_DOT;
                             break;
                     case 'e':
                     case 'E':
                             ++sz;
                             state=stBEFORE_EXPONENTE;
                             break;
                     case 0  : 
                              state = stEND;
                              break;
                     default:
                             throw UAKGQuery2::InvalidInputFormat();
                   }
                   break;
     case stAFTER_DOT:
                  afterDotDigits_=readDigits(sz,
                                             (nBeforeDotDigits_==nLeading0s_),
                                             nAfterDotDigits_);
                   switch(*sz) {
                     case 'e':
                     case 'E':
                             ++sz;
                             state=stBEFORE_EXPONENTE;
                             break;
                     default:
                             state=stEND;
                   }
     case stBEFORE_EXPONENTE:
                   expSign_=readSign(sz);
                   expDigits_=readDigits(sz,1,nExpDigits);
                   exponente_=atoi(expDigits_);
                   if (expSign_=='-') exponente_=-exponente_;
                   state=stEND;
     case stEND:
               trim(sz);
               if(*sz != 0) throw UAKGQuery2::InvalidInputFormat();
               break;      
   }
 }

}

