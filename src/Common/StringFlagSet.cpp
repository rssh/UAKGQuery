#ifndef __Common_StringFlagSet_h
#include <Common/StringFlagSet.h>
#endif

#ifdef HAVE_CSTRING
#include <cstring>
#endif

#include <stdio.h>
using namespace GradSoft;

/*
 * part of UAKGQuery implementation.
 * (C) GradSoft, 2000
 * author: Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>
 * $Id: StringFlagSet.cpp,v 1.14 2008-05-31 18:17:06 rssh Exp $
 */

StringFlagSet::StringFlagSet(const StringFlagSet* parent_p)
 :parent_p_(parent_p),
  storage_(),
  rwlock_()
{
}

StringFlagSet::~StringFlagSet()
{
 WriteLocker wl(rwlock_);
 StorageIteratorType it = storage_.begin();
 std::deque<const char*> toErase;
 while(it!=storage_.end()) {
   const char* tmp = (*it).first;
   toErase.push_back(tmp);
   ++it;
 }
 storage_.erase(storage_.begin(),storage_.end());
 for(std::deque<const char*>::iterator it1=toErase.begin(); it1!=toErase.end();
      ++it1) {
    delete[] const_cast<char*>(*it1);
 }
}

/**
 * parse string as set of flags, according with next BNF grammar:
 *   
 *   FlagDefs: 
 *            |
 *             FlagDef ',' FlagDefs
 *            ;
 *
 *   FlagDef: FlagName
 *           |
 *            FlagName '=' Value
 *           ;
 *
 *   Value:
 *          IntValue
 *         |
 *          StringValue
 *         ;
 *
 *   FlagName: ([a-z]|[0-9])*
 *   IntValue: [0-9]*
 *   StringValue: ([^,]|[\,])*
 *               ;
 *
 */
void  StringFlagSet::parse(const char* str)
{
  parseFlagDefs(str);
}


int   StringFlagSet::get_int(const char* name) const
{
 bool found;
 StorageConstIteratorType it = find(name,found);
 if (found) {
     return (*it).second.getIntValue();
 }else{
     return 0;
 }
}

const char* StringFlagSet::get_sz(const char* name) const
{
 bool found;
 StorageConstIteratorType it = find(name,found);
 if (found) {
   return (*it).second.getStringValue_p();
 }else{
   return NULL;
 }
}

#ifdef FUNCTION
#undef FUNCTION
#endif
#define FUNCTION "StringFlagSet::parseFlagDefs"
void  StringFlagSet::parseFlagDefs(const char*& str)
{
 while (*str!='\0') {
   FlagDef flagDef = parseFlagDef(str);
   addFlagDef(flagDef); 
 }
}
#undef FUNCTION


#define FUNCTION "StringFlagSet::parseFlagDef"
StringFlagSet::FlagDef  StringFlagSet::parseFlagDef(const char*& str)
{
 FlagDef retval;
 if (*str=='\0') throw StringFlagSet::ParseError("unexpected \\0");
 retval.name = parseName(str);
 if (*str=='=') {
   ++str;
   retval.value = parseValue(str);
 }else if(*str==',') {
   retval.value.frs = NULL;
   retval.value.snd = NULL;
   ++str;
 }else if(*str=='\0') {
   retval.value.frs = NULL;
   retval.value.snd = NULL;
 }else{
   throw StringFlagSet::ParseError("unexpected symbol");
 }
 return retval;
}
#undef FUNCTION

#define FUNCTION "StringFlagSet::parseName"
StringFlagSet::Chunk StringFlagSet::parseName(const char*& str)
{
 Chunk retval;
 retval.frs=(char*)str;
 while(*str!='\0' && *str!='=' && *str!=',') {
      ++str;
 }
 retval.snd=(char*)str;
 return retval;
}
#undef FUNCTION

#define FUNCTION "StringFlagSet::parseValue"
StringFlagSet::Chunk StringFlagSet::parseValue(const char*& str)
{
 Chunk retval;
 retval.frs=(char*)str;
 while(*str!='\0' && *str!=',') {
   if (*str=='\\') {
      ++str;
      if (*str=='\0') break;
   }
   ++str;
 }
 retval.snd=(char*)str;
 return retval;
}
#undef FUNCTION

void StringFlagSet::addFlagDef(StringFlagSet::FlagDef& flagDef)
{
 if (flagDef.name.frs==NULL) return;
 int nameBuffLen = flagDef.name.snd - flagDef.name.frs + 1 ;
 char* nameBuff = new char[nameBuffLen];
 strncpy(nameBuff,flagDef.name.frs,nameBuffLen);
 nameBuff[nameBuffLen-1]='\0';
 if (flagDef.value.frs!=NULL) {
       int valueBuffLen = flagDef.value.snd - flagDef.value.frs;
       char* valueBuff = new char[valueBuffLen+1];
       // we must do unescape here 
       char* buffp = valueBuff;
       char* vp = flagDef.value.frs;
       while(vp!=flagDef.value.snd) {
         if (*vp=='\\') {
           ++vp;
           if (*vp=='\0' || vp==flagDef.value.snd) {
             *buffp++='\\';
             break;
           }
         }else{
           *buffp++=*vp++;
         }
       }
       *buffp='\0';
       addString(nameBuff,valueBuff);
       delete[] valueBuff;
 }else{
       addInt(nameBuff,1);
 }
 // note: name ownership is passed to storage
}

// note: name ownership is passed to storage
void StringFlagSet::addString(const char* name, const char* value)
{
   WriteLocker wl(rwlock_); 
   storage_[name]=FlagValue(value); 
}

// note: name ownership is passed to storage
void StringFlagSet::addInt(const char* name, int value)
{
 WriteLocker wl(rwlock_); 
 storage_[name]=FlagValue(value); 
}


StringFlagSet::FlagValue::FlagValue(const char* value)
{
 char* endvalue;
 long x = strtol(value,&endvalue,10);
 if (endvalue==value) {
      is_int_ = false;
      int_value_ = 0;
 }else{
      is_int_ = true;
      int_value_ = (int)x;
 }
 sz_value_ = new char[strlen(value)+1];
 strcpy(sz_value_,value);
}

StringFlagSet::FlagValue::FlagValue(int value)
{
 char buff[10]; 
 memset(buff,0,10);
 sprintf(buff,"%d",value);
 is_int_=true; 
 int_value_=value;
 sz_value_ = new char[strlen(buff)+1];
 strcpy(sz_value_,buff);
}


StringFlagSet::FlagValue::FlagValue(const StringFlagSet::FlagValue& x)
     :is_int_(false),int_value_(0),sz_value_(NULL)
{
 setValue(x);
}

StringFlagSet::FlagValue::~FlagValue()
{
 if(sz_value_!=NULL) delete[] sz_value_;
}

void StringFlagSet::FlagValue::setValue(const StringFlagSet::FlagValue& x)
{
 if (sz_value_!=NULL) delete[] sz_value_;
 is_int_ = x.is_int_;
 int_value_ = x.int_value_;
 if (x.sz_value_ == NULL) {
        sz_value_ = NULL;
 }else{
        sz_value_ = new char[strlen(x.sz_value_)+1];
        strcpy(sz_value_,x.sz_value_);
 }
}

#define FUNCTION "StringFlagSet::printChunk"
void StringFlagSet::printChunk(std::ostream& out, const StringFlagSet::Chunk& chunk)
{
 if (chunk.frs!=NULL && chunk.snd!=NULL) {
   char* cur=chunk.frs;
   while(cur!=chunk.snd) {
	   out << (*cur);
	   ++cur;
   }
 }else if (chunk.frs==NULL && chunk.snd==NULL) {
   out << "<NULL chunk>" ;
 }else{
   out << "<incorrect chunk>" ;
 }
}
#undef FUNCTION

void StringFlagSet::print(std::ostream& out)
{
 for(StorageConstIteratorType it=storage_.begin();it!=storage_.end();++it) {
   out << "(" << (*it).first << "," << (*it).second.getStringValue_p() << ")" << std::endl;
   
 }
}

