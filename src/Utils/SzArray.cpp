#ifndef __Utils_SzArray_h
#include <Utils/SzArray.h>
#endif

#ifdef _MSC_VER
#include <string.h>
#pragma warning(disable:4786)
#else
#include <strings.h>
#endif

#ifndef assert
#include <assert.h>
#endif

SzArray::SzArray( const char* in_s, const char delim )
{
   int len=0;
   for( unsigned int i=0; i<strlen(in_s); i++ )
      if( in_s[i] == delim ) len++;

   strzs_.reserve(++len);
   strzs_.resize(len);

   int index = 0;

   const char* left=in_s;
   const char* right=strchr(in_s,delim);

   int strzsIndex = 0;
   while(right) {
     while(*left==' ')++left;
     if (left==right) {
       // empty field : error ?
       //   now, simple create empty field;
       strzs_[strzsIndex]=new char[1];
       strzs_[strzsIndex][0]='\0';
     }else{
       const char* right1=right-1; 
       while(*right1==' ') --right1;
       ++right1;
       strzs_[strzsIndex]=new char[right1-left+1];
       strncpy( strzs_[strzsIndex], left, right1-left);
       strzs_[strzsIndex][right1-left]='\0';
     } 
     ++strzsIndex;
     left=right+1;
     right=strchr(left,delim);
   }
   if (left) {
     while(*left==' ') ++left;
     if (*left=='\0') {
       // empty field 
       strzs_[strzsIndex]=new char[1];
       strzs_[strzsIndex][0]='\0';
     }else{
       right=left+strlen(left)-1;
       while(*right==' ') --right;
       ++right;
       strzs_[strzsIndex]=new char[right-left+1];
       strncpy( strzs_[strzsIndex], left, right-left);
       strzs_[strzsIndex][right-left]='\0';
     }
   }

}

void SzArray::append_sz_nodup(char* in_s)
{ strzs_.push_back(in_s); }

void SzArray::append_sz_dup(const char* in_s)
{ 
	assert(in_s!=NULL);
	char* s1=new char[strlen(in_s)+1];
	strcpy(s1,in_s);
	strzs_.push_back(s1); 
}


void SzArray::append_szarray(const SzArray& in_arr)
{
	for( int i=0; i<in_arr.size(); i++ )
		append_sz_dup( in_arr.get_sz_at(i) );
}
