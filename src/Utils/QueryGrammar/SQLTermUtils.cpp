
#include <Utils/SQLTerm.h>
#include <Utils/QueryGrammar/SQLParser.h>

#include <assert.h>


#include <strstream>
using namespace std;

inline SQLTerm& if_term(SQLTerm* t)
{
  return (t ? (*t) : 
          * SQLParser::getCurrentParserPtr()->yy_createConstStringTerm("")
         );
  // it is hack, but correct: new term is deleted by current memory tracker
  //  TODO: after creation of yayacc, put constant here
}

extern "C"
void* yy_createSelectSQLTerm(void* x1,void* x2,void*x3,void*x4, void* x5)
{
  return (void*)SQLParser::getCurrentParserPtr()->
               yy_createSelectSQLTerm((SQLTerm*)x1,(SQLTerm*)x2,
                                      (SQLTerm*)x3,(SQLTerm*)x4, (SQLTerm*)x5);
}

SQLTerm* SQLParser::yy_createSelectSQLTerm(SQLTerm* t1,SQLTerm* t2,
                                           SQLTerm* t3,SQLTerm* t4, SQLTerm* t5)
{
 SQLTerm* term;
 SQLTerm** members = new SQLTerm*[4];
 SQLTerm::Flawor flawor;
 SQLTerm* selectOptTerm = t1;
 if (selectOptTerm->isSelectOptAll()) {
   flawor=SQLTerm::FlaworSelectOptAll;
 }else if (selectOptTerm->isSelectOptDistinct()) {
   flawor=SQLTerm::FlaworSelectOptDistinct;
 }else{
   assert(0);
 }
 delete selectOptTerm;
 members[0]=t2;
 members[1]=t3;
 members[2]=t4;
 members[3]=t5;
 term=SQLTerm::createSQLTerm(memoryTracker_,
                             SQLTerm::SelectExpr,flawor,4,members,NULL);
 return term;
}

extern "C"
void* yy_createSelectOptTerm(int x)
{
  return SQLParser::getCurrentParserPtr()->yy_createSelectOptTerm(x);
}

SQLTerm* SQLParser::yy_createSelectOptTerm(int x)
{
 SQLTerm* term;
 if (x!=0) {
   term=SQLTerm::createSQLTerm(memoryTracker_,SQLTerm::SelectOptAll,
                                     SQLTerm::UndefinedFlawor,0,NULL,NULL);
 }else{
   term=SQLTerm::createSQLTerm(memoryTracker_,SQLTerm::SelectOptDistinct,
                                     SQLTerm::UndefinedFlawor,0,NULL,NULL);
 }
 return term;
}

extern "C"
void* yy_createTableExprSQLTerm(void* x1, void* x2, void* x3, void* x4)
{
 return (void*)SQLParser::getCurrentParserPtr()->
                   yy_createTableExprSQLTerm((SQLTerm*)x1,(SQLTerm*)x2,
                                          (SQLTerm*)x3,(SQLTerm*)x4);
}

SQLTerm* SQLParser::yy_createTableExprSQLTerm(SQLTerm* x1, SQLTerm* x2, 
                                              SQLTerm* x3, SQLTerm* x4)
{
 SQLTerm** members = new SQLTerm*[4];
 members[0]=x1;
 members[1]=x2;
 members[2]=x3;
 members[3]=x4;
 SQLTerm* term=SQLTerm::createSQLTerm(memoryTracker_,
                             SQLTerm::TableExpr,
                             SQLTerm::UndefinedFlawor,4,members,NULL);
 return term;
}

extern "C"
void* yy_createConstStringTerm(const char* x)
{
 return (void*)SQLParser::getCurrentParserPtr()->yy_createConstStringTerm(x);
}

SQLTerm* SQLParser::yy_createConstStringTerm(const char* x)
{
 return SQLTerm::createConstStringTerm(memoryTracker_,x);
}

SQLTerm* SQLParser::yy_createStringTerm(const char* x)
{
 return SQLTerm::createStringTerm(memoryTracker_,x);
}

extern "C"
void* yy_catStringTerm3_tct(void* x1,const char* c2,void* x3)
{
 return (void*)SQLParser::getCurrentParserPtr()->
                       yy_catStringTerm3_tct((SQLTerm*)x1,c2,(SQLTerm*)x3);
}

SQLTerm* SQLParser::yy_catStringTerm3_tct(SQLTerm* t1, const char* c2, 
                                          SQLTerm* t3)
{
 ostrstream ostr;  
 ostr << if_term(t1) << ' ' << c2 << if_term(t3) << '\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t1;
 delete t3;
 return t;
}

extern "C"
void* yy_catStringTerm3_ctc(const char* c1,void* x2,const char* c3)
{
  return (void*)SQLParser::getCurrentParserPtr()->yy_catStringTerm3_ctc(
                                                      c1,(SQLTerm*)x2,c3);
}

SQLTerm* SQLParser::yy_catStringTerm3_ctc(const char* c1, SQLTerm* t2, 
                                       const char* c3)
{
 ostrstream ostr;  
 ostr << c1  << ' ' << if_term(t2) << ' ' << c3 << '\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t2;
 return t;
}

extern "C"
void* yy_catStringTerm2_ct(const char* c1, void* x2)
{
 return (void*)SQLParser::getCurrentParserPtr()->
                               yy_catStringTerm2_ct(c1,(SQLTerm*)x2);
}

SQLTerm* SQLParser::yy_catStringTerm2_ct(const char* c1, SQLTerm* t2)
{
 ostrstream ostr;  
 ostr << c1 << ' ' << if_term(t2)  << '\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t2;
 return t;
}

extern "C"
void* yy_catStringTerm2_tc(void* x1, const char* c2)
{
 return (void*)SQLParser::getCurrentParserPtr()->
                                  yy_catStringTerm2_tc((SQLTerm*)x1,c2);
}

SQLTerm* SQLParser::yy_catStringTerm2_tc(SQLTerm* t1, const char* c2)
{
 ostrstream ostr;  
 ostr << if_term(t1) << ' ' << c2 << '\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t1;
 return t;
}

extern "C"
void* yy_catStringTerm2_tt(void* x1, void* x2)
{
 return (void*)SQLParser::getCurrentParserPtr()->
                          yy_catStringTerm2_tt((SQLTerm*)x1,(SQLTerm*)x2);
}

SQLTerm* SQLParser::yy_catStringTerm2_tt(SQLTerm* t1, SQLTerm* t2)
{
 ostrstream ostr;  
 ostr << if_term(t1) << ' ' << if_term(t2) << '\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t1;
 delete t2;
 return t;
}

extern "C"
void* yy_catStringTerm4_tctc(void* x1,const char* c2, void* x3,const char* c4)
{
 return (void*)SQLParser::getCurrentParserPtr()->
                yy_catStringTerm4_tctc((SQLTerm*)x1,c2,(SQLTerm*)x3,c4);
}

SQLTerm* SQLParser::yy_catStringTerm4_tctc(SQLTerm* t1,const char* c2, 
                                           SQLTerm* t3,const char* c4)
{
 ostrstream ostr;  
 ostr << if_term(t1) << ' ' << c2 << ' ' << if_term(t3) << ' ' << c4 << '\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t1;
 delete t3;
 return t;
}

extern "C"
void* yy_catStringTerm4_tctt(void* x1,const char* c2, void* x3,void* x4)
{
 return (void*)SQLParser::getCurrentParserPtr()->
            yy_catStringTerm4_tctt((SQLTerm*)x1,c2,(SQLTerm*)x3,(SQLTerm*)x4);
}

SQLTerm* SQLParser::yy_catStringTerm4_tctt(SQLTerm* t1,const char* c2,
                                           SQLTerm* t3,SQLTerm* t4)
{
 ostrstream ostr;  
 ostr << if_term(t1)<<' '<<c2<<' '<<if_term(t3)<<' '<<if_term(t4)<<'\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t1;
 delete t3;
 delete t4;
 return t;
}

extern "C"
void* yy_catStringTerm5_tctct(void* x1,const char* c2,void* x3,
                              const char* c4,void* x5)
{
 return (void*)SQLParser::getCurrentParserPtr()->
       yy_catStringTerm5_tctct((SQLTerm*)x1,c2,(SQLTerm*)x3,c4,(SQLTerm*)x5);
}

SQLTerm* SQLParser::yy_catStringTerm5_tctct(SQLTerm* t1, const char* c2,
                                         SQLTerm* t3, const char* c4,
                                         SQLTerm* t5)
{ 
 ostrstream ostr;  
 ostr << if_term(t1)<<' '<<c2<<' '<<if_term(t3)<< ' '<<(c4) << ' '
      << if_term(t5) << '\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t1;
 delete t3;
 delete t5;
 return t;
}

extern "C"
void* yy_catStringTerm6_cttttc(const char* c1,void*x2 ,void*x3,
                               void* x4,void* x5,const char* c6)
{
 return (void*)SQLParser::getCurrentParserPtr()->
          yy_catStringTerm6_cttttc(c1,(SQLTerm*)x2 ,(SQLTerm*)x3,
                                   (SQLTerm*)x4,(SQLTerm*)x5,c6);
}

SQLTerm* SQLParser::yy_catStringTerm6_cttttc(const char* c1, SQLTerm* t2,
                                  SQLTerm* t3, SQLTerm* t4, SQLTerm* t5,
                                   const char* c6)
{
 ostrstream ostr;  
 ostr << c1 << ' ' << if_term(t2) << ' ' << if_term(t3) << ' ' 
      << if_term(t4) << ' '
      << if_term(t5) << ' ' << c6 << '\0';
 SQLTerm* t = SQLTerm::createStringTerm(memoryTracker_,ostr.str());
 ostr.rdbuf()->freeze(0);
 delete t2;
 delete t3;
 delete t4;
 delete t5;
 return t;
}

