#ifndef __Utils_SQLTextUtils_h
#define __Utils_SQLTextUtils_h

#ifndef __Utils_SzArray_h
#include <Utils/SzArray.h>
#endif

class SQLTextUtils
{

public:

 /**
  * Removes string constants from input string separated by quotes
  *  (new string is allocated by new[] operator)
  */
 static char* generate_new_str_without_strconsts(const char*, const char);

 /**
  * select f1, f2 from table1 where f3=f3_val and f1=f1_val order by f2 -->
  * SzArray : ["f1, f2", "table1", "f3=f3_val and f1=f1_val", "f2"]
  *  (input statement must be started from 'select' keyword)
  */
 static SzArray* create_sql_parts_szarray( const char* );

 /**
  * Parse in_s by key word (excluding them) from in_key_list,
  * skipping const string ('...');
  * all of them are optional;
  * return empty string in case of absent key word
  */
 static SzArray* create_sql_parts_szarray( const char* in_s, 
                                           const SzArray& in_key_list );

 /**
  * return true, if sz if identificator.
  *@param sz - string to text
  *@returns true, if sz is identificator
  */
 static bool isIdent(const char* sz);


 /**
  * replaces in src all 'what' with 'with'.
  *@param src - source string (String_var.inout)
  *@param what - string to replace
  *@param with - string to replace with
  */
 static void replace(char*& src, const char* what, const char* with);

 /**
  * replaces in src all 'what' with 'with' except if before 'what' goes escape
  * symbol 'esc'.
  *@param src - source string (String_var.inout)
  *@param what - string to replace
  *@param with - string to replace with
  */
 static void replace(char*& src, const char* what, const char* with
     , char esc);

 /**
  * concatenates str1, str2 and str3.
  *@param str1 - source string
  *@param str2 - source string
  *@param str3 - source string
  *@returns str1 || str2 || str3
  */
 static char* concat(const char* str1, const char* str2, const char* str3 = "");

 

};

#endif
