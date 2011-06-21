#ifndef __Utils_QueryGrammar_SQLScanner_h
#define __Utils_QueryGrammar_SQLScanner_h

/**
 * lexical scanner for grammar of SQL query.
 * part of UAKG Query Service
 * (C) GradSoft, 2000
 * $Id: SQLScanner.h,v 1.6 2001-02-06 12:24:36 yad Exp $
 **/


#ifndef __Utils_QueryGrammar_SQLSymbolTable_h
#include <Utils/QueryGrammar/SQLSymbolTable.h>
#endif

#include <iostream>

/**
 * class for parsing SQL query.
 * note, that we create instance of this class for each query.
 **/
class SQLScanner
{
public:

 struct ParseError
 {
  const char* message_;
  int pos_;
  ParseError(const char* message, int pos)
   :message_(message),pos_(pos) 
  { std::cerr << "SQLScanner::ParseError constructor" << std::endl; }
 };

private:

 const char* buff_;
 const char* curr_buff_;

 char* token_;
 char* curr_token_;

 static SQLSymbolTable symbolTable_;
 int   state_;

 int   getPos() { return curr_buff_ - buff_ ; }

public:

 SQLScanner(const char* buff);
 ~SQLScanner();

 int getNextToken();
 const char* getLastTokenValue() const;

private:

 int readToken();
 void cleanToken();

 char readChar();
 void unreadChar();
 void decCurrentToken();

 bool atEnd();

 void throwParseError(const char* msg);

};

#endif
