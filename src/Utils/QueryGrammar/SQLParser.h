#ifndef __Utils_QueryGrammar_SQLParser_h
#define __Utils_QueryGrammar_SQLParser_h

#ifndef __Utils_QueryGrammar_SQLScanner_h
#include <Utils/QueryGrammar/SQLScanner.h>
#endif

#ifndef __Utils_SQLTerm_h
#include <Utils/SQLTerm.h>
#endif

#ifndef __Utils_SQLTermMemoryTracker_h
#include <Utils/SQLTermMemoryTracker.h>
#endif

#ifndef __GradSoft_Threading_h
#include <GradSoft/Threading.h>
#endif
#include <iostream>

class SQLParser
{
private:

  SQLScanner scanner_;
  SQLTermMemoryTracker memoryTracker_;
  static GradSoft::Mutex     parseMutex_;
  const char*      errorMessage_;

public:

  SQLParser(const char* buff)
   :scanner_(buff),
    memoryTracker_(),
    errorMessage_(NULL)
  {}
  
  ~SQLParser()
  {
    std::cerr << "~SQLParser" << std::endl;
  }

  SQLTerm* parse();

  bool     wasError() const { return errorMessage_ != NULL; }
  const char* getErrorMessage() const
                            { return errorMessage_; }

  //
  // return active SQLParser.
  // (aviable only during time of yyparse)  
  // in future will return SQLParser, binded to
  //  current thread.
  //
  static SQLParser* getCurrentParserPtr();

private:

  SQLParser();
  SQLParser(const SQLParser& x);
  SQLParser& operator=(const SQLParser& x);

public:

// functions, called from yyparse:

   SQLTerm* yy_createSelectSQLTerm(SQLTerm* t1,SQLTerm* t2,
                                   SQLTerm* t3,SQLTerm* t4, SQLTerm* t5);

   SQLTerm* yy_createSelectOptTerm(int x);

   SQLTerm* yy_createTableExprSQLTerm(SQLTerm* x1, SQLTerm* x2, 
                                  SQLTerm* x3, SQLTerm* x4);

   SQLTerm* yy_createConstStringTerm(const char* x);

   SQLTerm* yy_createStringTerm(const char* x);

   SQLTerm* yy_catStringTerm3_tct(SQLTerm* t1, const char* c2, SQLTerm* t3);

   SQLTerm* yy_catStringTerm3_ctc(const char* c1, SQLTerm* t2, 
                                  const char* c3);

   SQLTerm* yy_catStringTerm2_ct(const char* c1, SQLTerm* t2);

   SQLTerm* yy_catStringTerm2_tc(SQLTerm* t1, const char* c2);

   SQLTerm* yy_catStringTerm2_tt(SQLTerm* t1, SQLTerm* t2);

   SQLTerm* yy_catStringTerm4_tctc(SQLTerm* t1,const char* c2, 
                                   SQLTerm* t3,const char* c4);

   SQLTerm* yy_catStringTerm4_tctt(SQLTerm* t1,const char* c2,
                                   SQLTerm* t3,SQLTerm* t4);

   SQLTerm* yy_catStringTerm5_tctct(SQLTerm* t1, const char* c2,
                                    SQLTerm* t3, const char* c4,
                                    SQLTerm* t5);


   SQLTerm* yy_catStringTerm6_cttttc(const char* c1, SQLTerm* t2,
                                     SQLTerm* t3, SQLTerm* t4, SQLTerm* t5,
                                     const char* c6);

   void  setParseError(const char* message)
   {
     errorMessage_ = message;
   }

};

#endif
