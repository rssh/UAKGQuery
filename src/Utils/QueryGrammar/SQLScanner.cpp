#include <Utils/QueryGrammar/SQLScanner.h>
#include <Utils/QueryGrammar/SQLParser.h>

#include <Utils/QueryGrammar/y.tab.h>

/**
 * state definitions for state 
 */
#define IN_SPACE     1
#define IN_STRING    2
#define IN_C_COMMENT 3
#define IN_DIGITS    4
#define IN_IDENT     5
#define IN_APPROXNUM_DOT 6
#define IN_APPROXNUM_E 7
#define IN_APPROXNUM_E_SIGN 8

static SQLSymbolTable sqlSymbolTable;

SQLScanner::SQLScanner(const char* buff)
{
 buff_ = buff;
 curr_buff_ = buff;
 token_ = new char[strlen(buff)+1];
 token_[0]='\0';
 curr_token_=token_;
 state_ = IN_SPACE;
}

SQLScanner::~SQLScanner()
{
  delete [] token_;
}

const char* SQLScanner::getLastTokenValue() const
{
 return token_;
}

int SQLScanner::getNextToken()
{
 int retval = readToken();
 if (retval == IDENT) {
   retval = sqlSymbolTable.checkSymbol(token_);
   return (retval== -1) ? IDENT : retval;
 }
 return retval; 
}


int SQLScanner::readToken()
{
try {
 cleanToken();
 char ch, ch1;
 for(;;){ 
  ch = readChar(); 
  switch(state_) {
    case IN_SPACE:
           switch(ch) {
             case ' ':
             case '\t':
             case '\r':
             case '\n':
                  decCurrentToken(); // erase '<'
                  break;
             case '!':
                 ch1 = readChar();
                 if (ch1=='=') return NEQ;
                 throwParseError("Unexpected Symbol");
             case '"':
             case '#':
             case '$':
             case '%':
             case '&':
                 throwParseError("Unexpected Symbol");
             case '\'':
                 state_ = IN_STRING;
                 break;
             case '(':
             case ')':     
             case '*':     
             case '+':     
             case ',':     
             case '-':     
             case '.':     
                 return (int)((unsigned char)ch);
             case '/':
                 ch1 = readChar();
                 if (ch1=='*') {
                   state_ = IN_C_COMMENT;
                   decCurrentToken();
                   decCurrentToken();
                   break;
                 }else{
                    return (int)((unsigned char)ch);
                 }
                 break;
             case '0':
             case '1':
             case '2':
             case '3':
             case '4':
             case '5':
             case '6':
             case '7':
             case '8':
             case '9':
                 state_ = IN_DIGITS ;
                 break;
             case ':':
             case ';':
                 return (int)((unsigned char)ch);
             case '<':
                  ch1=readChar(); 
                  if (ch1=='>') {
                     return NEQ;
                  }else if(ch1=='='){
                     return LESS_EQ;
                  }  
                  unreadChar(); 
                  return LESS; 
             case '=':
                  return EQ;
             case '>':
                  ch1=readChar(); 
                  if (ch1=='=') {
                     return GT_EQ;
                  }
                  unreadChar(); 
                  return GT; 
             case '?':
                  return '?'; 
             case '@':
                  return '@'; 
             case 'A':
             case 'B':
             case 'C':
             case 'D':
             case 'E':
             case 'F':
             case 'G':
             case 'H':
             case 'I':
             case 'J':
             case 'K':
             case 'L':
             case 'M':
             case 'N':
             case 'O':
             case 'P':
             case 'Q':
             case 'R':
             case 'S':
             case 'T':
             case 'U':
             case 'V':
             case 'W':
             case 'X':
             case 'Y':
             case 'Z':
                  state_ = IN_IDENT;
                  break;
             case '[':
             case '\\':
             case ']':
                   throwParseError("Unexpected symbol");
             case 'a':
             case 'b':
             case 'c':
             case 'd':
             case 'e':
             case 'f':
             case 'g':
             case 'h':
             case 'i':
             case 'j':
             case 'k':
             case 'l':
             case 'm':
             case 'n':
             case 'o':
             case 'p':
             case 'q':
             case 'r':
             case 's':
             case 't':
             case 'u':
             case 'v':
             case 'w':
             case 'x':
             case 'y':
             case 'z':
                  state_ = IN_IDENT;
                  break;
             case '{':
                   throwParseError("Unexpected symbol");
             case '|':
                   ch1 = readChar();
                   if (ch1=='|') {
                     return CONCAT_OP;
                   }
                   throwParseError("Unexpected symbol");
              case '\0':
                     return 0; 
             default:
                   throwParseError("Unexpected symbol");
           }
           break;
     case IN_STRING: 
           if (ch=='\'') {
              ch1=readChar();
              if (ch1=='\'') {
                 //decCurrentToken();  // '' -> ' in yylval
                 //  - do not do this, unleww we have no special term for
                 //    string-constant in SQL
              }else if (ch1=='\0') {
                 state_ = IN_SPACE;
                 return STRING;
              }else{
                 unreadChar();
                 //decCurrentToken();  // erase ending '
                 //  - do not do this, unleww we have no special term for
                 //    string-constant in SQL
                 state_ = IN_SPACE;
                 return STRING;
              }
           }else if (ch=='\0') {
              throwParseError("Unterminated string");
           }else{
              // do nothing
           } 
           break;
      case IN_C_COMMENT:
           decCurrentToken();
           if (ch=='*') {
              ch1=readChar();
              if (ch1=='/') {
                state_ = IN_SPACE; 
                decCurrentToken();
              }else{
                unreadChar();
              }
           }else if (ch=='\0') {
              throwParseError("Unterminated comment");
           }
           break;
      case IN_DIGITS:
           switch(ch) {
             case '0':
             case '1':
             case '2':
             case '3':
             case '4':
             case '5':
             case '6':
             case '7':
             case '8':
             case '9':
                      break;
             case '.':
                      state_ = IN_APPROXNUM_DOT;
                      break;
             case 'e':
             case 'E':
                      state_ = IN_APPROXNUM_E;
                      break;
             case '\0':
                      state_ = IN_SPACE;
                      return INTNUM; 
             default:
                      unreadChar();
                      state_ = IN_SPACE;
                      return INTNUM;
           } 
           break;
      case IN_IDENT:
           switch(ch) {
             case '0':
             case '1':
             case '2':
             case '3':
             case '4':
             case '5':
             case '6':
             case '7':
             case '8':
             case '9':
             case '_':
             case 'A':
             case 'B':
             case 'C':
             case 'D':
             case 'E':
             case 'F':
             case 'G':
             case 'H':
             case 'I':
             case 'J':
             case 'K':
             case 'L':
             case 'M':
             case 'N':
             case 'O':
             case 'P':
             case 'Q':
             case 'R':
             case 'S':
             case 'T':
             case 'U':
             case 'V':
             case 'W':
             case 'X':
             case 'Y':
             case 'Z':
             case 'a':
             case 'b':
             case 'c':
             case 'd':
             case 'e':
             case 'f':
             case 'g':
             case 'h':
             case 'i':
             case 'j':
             case 'k':
             case 'l':
             case 'm':
             case 'n':
             case 'o':
             case 'p':
             case 'q':
             case 'r':
             case 's':
             case 't':
             case 'u':
             case 'v':
             case 'w':
             case 'x':
             case 'y':
             case 'z':
                   break;
             case '\0':
                  state_ = IN_SPACE;
                  return IDENT;
             default:
                  unreadChar();
                  state_ = IN_SPACE;
                  return IDENT;
          }
          break;
     case IN_APPROXNUM_DOT:
          switch(ch){
             case '0':
             case '1':
             case '2':
             case '3':
             case '4':
             case '5':
             case '6':
             case '7':
             case '8':
             case '9':
                     break;
             case '\0': 
                 state_ = IN_SPACE;
                 return APPROXNUM;
             default:
                 unreadChar();
                 state_ = IN_SPACE;
                 return APPROXNUM;
          } 
          break;
     case IN_APPROXNUM_E:
          switch(ch){
            case '+':
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               state_ = IN_APPROXNUM_E_SIGN;
               break; 
            case '\0':
                 throwParseError("APPROXNUM without exponent value");
            default:
               unreadChar();
               state_ = IN_SPACE;
               return APPROXNUM;
          } 
          break;
     case IN_APPROXNUM_E_SIGN:
          switch(ch){
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               break; 
            case '\0':
               state_ = IN_SPACE;
               return APPROXNUM;
            default:
               unreadChar();
               state_ = IN_SPACE;
               return APPROXNUM;
          } 
          break;
     default:
          throwParseError("Wrong scanner state");
  }
 }//for
}catch(const SQLScanner::ParseError& ex){
 SQLParser::getCurrentParserPtr()->setParseError(ex.message_);
 return 256; // error
}
}

bool SQLScanner::atEnd()
{
 return *curr_buff_ == '\0';
}

void SQLScanner::cleanToken()
{
 curr_token_=token_;
 *curr_token_ = '\0';
}

char SQLScanner::readChar()
{
 char retval;
 if ((retval=*curr_buff_) != '\0') {
   *curr_token_++=*curr_buff_++;
   *curr_token_='\0';
 }
 return retval; 
}

void SQLScanner::unreadChar()
{
 if (curr_buff_==buff_ || curr_token_==token_) {
    throwParseError(
      "Internal Error: call of SQLScanner::unreadChar in bad state"
    );
 } 
 --curr_buff_;
 *--curr_token_='\0';
}

void SQLScanner::decCurrentToken()  
{
 *--curr_token_='\0';
}

void SQLScanner::throwParseError(const char* msg)
{
  throw SQLScanner::ParseError(msg,(int)(curr_buff_ - buff_));
}

