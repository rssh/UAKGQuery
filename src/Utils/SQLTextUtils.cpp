#ifndef __Utils_SQLTextUtils_h
#include <Utils/SQLTextUtils.h>
#endif

#ifdef _MSC_VER
#include <string.h>
#else
#include <strings.h>
#endif

#ifndef __importCORBA_h
#include <importCORBA.h>
  // for somewhere defined bool on platforms, where bool is not yet supported
  //  by compiler ;)
#endif


#include <cassert>
#include <string>
#include <strstream>
using namespace std;

char* SQLTextUtils::generate_new_str_without_strconsts(const char* in_s, const char quote)
{
 char* szcurtailed = new char[strlen(in_s)+1];
 int i=0;
 bool szconst=false;
 for(unsigned int j=0; j<strlen(in_s); j++)
 {
  if( !szconst )
  { szcurtailed[i++]=in_s[j]; }
  if( in_s[j] == quote)
  { szconst = !szconst; }
 }
 szcurtailed[i]='\0';
 return szcurtailed;
}

SzArray* SQLTextUtils::create_sql_parts_szarray( const char* in_s )
{
 SzArray* szarr = new SzArray();

 // Step1 : parse string constants
 //
 char* szcurtailed = new char[strlen(in_s)+1];
 char* zsconst = new char[strlen(in_s)+1];
 int i=0;
 int k=0;
 bool quote=false;
 for(unsigned int j=0; j<strlen(in_s); j++)
 {
  if( quote )
  { zsconst[k++]=in_s[j];}
  else
  { szcurtailed[i++]=in_s[j]; }
  if( in_s[j] == '\'')
  { quote = !quote; }
 }
 szcurtailed[i]='\0';
 zsconst[k]='\0';

 // Step2: parse keywords
 //
 ostrstream tmpQPart;

 char* szconst_ptr = zsconst;
 char* clearQuery_ptr1 = szcurtailed+strlen("select ");
 char* clearQuery_ptr2 = strstr(clearQuery_ptr1, "from");
 for( ; clearQuery_ptr1<clearQuery_ptr2; clearQuery_ptr1++ )
 {
    tmpQPart << clearQuery_ptr1[0];
	 if( *clearQuery_ptr1=='\'' )
		 do
		 { tmpQPart << szconst_ptr[0]; }
		 while( *szconst_ptr++ != '\'' );
 }
 tmpQPart << '\0';
 szarr->append_sz_dup( tmpQPart.str() );
 tmpQPart.rdbuf()->freeze(0);
 tmpQPart.rdbuf()->pubseekpos(0,ios::out);

 clearQuery_ptr1 = clearQuery_ptr2 + strlen("from ");
 clearQuery_ptr2 = strstr(clearQuery_ptr1, "where");
 for( ; clearQuery_ptr1<clearQuery_ptr2; clearQuery_ptr1++ )
 {
   tmpQPart << clearQuery_ptr1[0];
   if( *clearQuery_ptr1=='\'' )
     do
     { tmpQPart << szconst_ptr[0]; }
     while( *szconst_ptr++ != '\'' );
 }
 tmpQPart << '\0';
 szarr->append_sz_dup( tmpQPart.str() );
 tmpQPart.rdbuf()->freeze(0);
 tmpQPart.rdbuf()->pubseekpos(0,ios::out);
 clearQuery_ptr1 = clearQuery_ptr2 + strlen("where ");

 clearQuery_ptr2 = strstr(clearQuery_ptr1, "order by ");
 if( clearQuery_ptr2 )
 {
	 for( ; clearQuery_ptr1<clearQuery_ptr2; clearQuery_ptr1++ )
	 {
		 tmpQPart << clearQuery_ptr1[0];
		 if( *clearQuery_ptr1=='\'' )
			 do
			 { tmpQPart << szconst_ptr[0]; }
			 while( *szconst_ptr++ != '\'' );
	 }
         tmpQPart << '\0';
	 szarr->append_sz_dup( tmpQPart.str() );
         tmpQPart.rdbuf()->freeze(0);
	 tmpQPart.rdbuf()->pubseekpos(0,ios::out);
	 clearQuery_ptr1 = clearQuery_ptr2 + strlen("order by ");
 }

 clearQuery_ptr2 = szcurtailed + strlen(szcurtailed);
 for( ; clearQuery_ptr1<clearQuery_ptr2; clearQuery_ptr1++)
 {
	 tmpQPart << clearQuery_ptr1[0];
	 if( *clearQuery_ptr1=='\'' )
		 do
		 { tmpQPart << szconst_ptr[0]; }
		 while( *szconst_ptr++ != '\'' );
 }
 tmpQPart << '\0';
 szarr->append_sz_dup( tmpQPart.str() );

 tmpQPart.rdbuf()->freeze(0);

 delete[] szcurtailed;
 delete[] zsconst;
 return szarr;
}

/**
 * Parse in_s by key word (excluding them) from in_key_list,
 * ignoring const string ('...');
 * all of them are optional;
 * return empty string in case of absent key word
 * 
 */
SzArray* SQLTextUtils::create_sql_parts_szarray( const char* in_s,
                                                 const SzArray& in_key_list)
{
 SzArray* szarr = new SzArray();

 // Step1 : parse string constants
 //
 // Here we remove string constants (delimited by ') and
 // store them in zsconst
 //
 char* szcurtailed = new char[strlen(in_s)+1];
 char* zsconst = new char[strlen(in_s)+1];
 unsigned int j;
 int i=0;
 int k=0;
 bool quote=false;
 for(j=0; j<strlen(in_s); j++)
 {
  if( quote )
  { zsconst[k++]=in_s[j];}
  else
  { szcurtailed[i++]=in_s[j]; }
  if( in_s[j] == '\'')
  { quote = !quote; }
 }
 szcurtailed[i]='\0';
 zsconst[k]='\0';

 // Step2: parse keywords
 //
 char* szconst_ptr = zsconst;

 char* clearQuery_ptr1;
 char* clearQuery_ptr2;
 ostrstream tmpQPart;
 clearQuery_ptr1 = szcurtailed;
 for( i=0, j=1; i<in_key_list.size()-1; i++, j++ )
 {
   // Make clearQuery_ptr1 point to the end of the current key word
   clearQuery_ptr1 += strlen( in_key_list[i] );
   // Make clearQuery_ptr2 point to the beginning of the next key word
   // presents in parsing string
   clearQuery_ptr2 = strstr(clearQuery_ptr1, in_key_list[j] );
   while( (j<in_key_list.size()-1) && (!clearQuery_ptr2) )
   {
	   j++;
	   clearQuery_ptr2 = strstr(clearQuery_ptr1, in_key_list[j] );
   }
   // cut the inner part repearing removed string cconstant
   for( ; clearQuery_ptr1<clearQuery_ptr2; clearQuery_ptr1++ )
   {
     tmpQPart << clearQuery_ptr1[0];
     if( *clearQuery_ptr1=='\'' )
     do
     { tmpQPart << szconst_ptr[0]; }
     while( *szconst_ptr++ != '\'' );
   }
   // add cut part to the array
   tmpQPart << '\0';
   szarr->append_sz_dup( tmpQPart.str() );
   // add empty strings to the array for all the key word
   // not found in incoming string
   tmpQPart.rdbuf()->freeze(0);
   tmpQPart.rdbuf()->pubseekpos(0,ios::out);
   tmpQPart << '\0';
   for( ; i<j-1; i++ )
   { szarr->append_sz_dup( tmpQPart.str() ); }
   tmpQPart.rdbuf()->freeze(0);
   tmpQPart.rdbuf()->pubseekpos(0,ios::out);
 }

 // the same process for the last key word
 clearQuery_ptr1 += strlen( in_key_list[i] );
 clearQuery_ptr2 = szcurtailed + strlen(szcurtailed);
 for( ; clearQuery_ptr1<clearQuery_ptr2; clearQuery_ptr1++)
 {
   tmpQPart << clearQuery_ptr1[0];
   if( *clearQuery_ptr1=='\'' )
   do
   { tmpQPart << szconst_ptr[0]; }
   while( *szconst_ptr++ != '\'' );
 }
 tmpQPart << '\0';
 szarr->append_sz_dup( tmpQPart.str() );

 tmpQPart.rdbuf()->freeze(0);
 delete[] szcurtailed;
 delete[] zsconst;

 return szarr;
}

bool SQLTextUtils::isIdent(const char* sz)
{
  for(;*sz;++sz){
     if ((*sz)>='A' && (*sz) <='Z') continue;
     if ((*sz)>='a' && (*sz) <='z') continue;
     if ((*sz)>='0' && (*sz) <='9') continue;
     return false;
  }
  return true;
}

void SQLTextUtils::replace(char*& src, const char* what, const char* with)
{
    assert(*what != '\0');
    char* next;
    if ((next = strstr(src, what)) && (*what != '\0')) {// some grace
        CORBA::String_var res = src;
        string buff = src;
        std::string::size_type whatLen = strlen(what);
        std::string::size_type curr = next - src;
        do {
            buff.replace(curr, whatLen, with);
        } while((curr = buff.find(what, curr)) != std::string::npos);
        res = CORBA::string_dup(buff.c_str());
        src = res._retn();
    }
};

void SQLTextUtils::replace(char*& src, const char* what, const char* with,
    char esc)
{
    assert(*what != '\0');
    char* next;
    if ((next = strstr(src, what)) && (*what != '\0')) {// some grace
        CORBA::String_var res = src;
        string buff = src;
        std::string::size_type whatLen = strlen(what);
        std::string::size_type curr = next - src;
        do {
            if (curr == 0 || buff[curr - 1] != esc) {
                buff.replace(curr, whatLen, with);
            }
        } while((curr = buff.find(what, curr)) != std::string::npos);
        res = CORBA::string_dup(buff.c_str());
        src = res._retn();
    }
};

char* SQLTextUtils::concat(const char* str1, const char* str2,
    const char* str3)
{
    int str1Len = strlen(str1);
    int str2Len = strlen(str2);
    int str3Len = strlen(str3);
    char* res = CORBA::string_alloc(str1Len + str2Len + str1Len + 1);
    strcpy(res, str1);
    strcpy(res + str1Len, str2);
    strcpy(res + str1Len + str2Len, str3);
	res[str1Len + str2Len + str3Len] = '\0';
    return res;
}

