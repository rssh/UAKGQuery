#ifndef __Utils_QueryGrammar_SQLParser_h
#include <Utils/QueryGrammar/SQLParser.h>
#endif

#include <Utils/QueryGrammar/y.tab.h>


static SQLScanner* currentScanner_p;
static SQLParser* currentParser_p;

extern "C" int yyparse();
extern "C" int yylex();
extern "C" void* yylval;
extern "C" void* yyval;

extern "C"
int yylex()
{
 int retval = currentScanner_p->getNextToken();
 if (retval==IDENT || retval==INTNUM || retval==APPROXNUM || retval==STRING) {
   yylval =   (void*) currentParser_p->
                  yy_createStringTerm(currentScanner_p->getLastTokenValue());
 }else{
   yylval = NULL;
 }
 return retval;
}

extern "C"
void yy_setParseError(const char* msg)
{
    currentParser_p->setParseError(msg);
}


SQLTerm* SQLParser::parse()
{
 GradSoft::MutexLocker ml(parseMutex_);
 currentScanner_p = &scanner_;
 currentParser_p = this;
 ::yyparse();
 currentParser_p = NULL;
 return (SQLTerm*)yyval;
}

SQLParser* SQLParser::getCurrentParserPtr()
{
  return currentParser_p;
}

GradSoft::Mutex SQLParser::parseMutex_;


