#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 3 "query.y"
#include <stdlib.h>
#include <stdio.h>
typedef void* YYType;
#define YYSTYPE YYType

#define YYDEBUG 1

void* yy_createSelectSQLTerm(void*,void*,void*,void*, void*);
void* yy_createSelectOptTerm(int);
void* yy_createTableExprSQLTerm(void*, void*, void*, void*);

void* yy_createConstStringTerm(const char*);

void* yy_catStringTerm3_tct(void*,const char*,void*); 
void* yy_catStringTerm3_ctc(const char*,void*,const char*); 
void* yy_catStringTerm2_ct(const char*, void*); 
void* yy_catStringTerm2_tc(void*, const char*); 
void* yy_catStringTerm2_tt(void*, void*); 
void* yy_catStringTerm4_tctc(void*,const char*, void*,const char*); 
void* yy_catStringTerm4_tctt(void*,const char*, void*,void*); 
void* yy_catStringTerm5_tctct(void*,const char*,void*,const char*,void*); 
void* yy_catStringTerm6_cttttc(const char*,void*,void*,void*,void*,const char*);

void yy_setParseError(const char* s);


#line 39 "y.tab.c"
#define OR 257
#define AND 258
#define NOT 259
#define ALL 260
#define ANY 261
#define BETWEEN 262
#define BY 263
#define DISTINCT 264
#define ESCAPE 265
#define EXISTS 266
#define FROM 267
#define GROUP 268
#define HAVING 269
#define ORDER 270
#define SELECT 271
#define SOME 272
#define WHERE 273
#define IN 274
#define INDICATOR 275
#define INTNUM 276
#define IS 277
#define DBNULL 278
#define USER 279
#define WITH 280
#define KEY 281
#define ASC 282
#define DESC 283
#define LESS 284
#define LESS_EQ 285
#define GT 286
#define GT_EQ 287
#define EQ 288
#define NEQ 289
#define LIKE 290
#define CONCAT_OP 291
#define IDENT 292
#define APPROXNUM 293
#define STRING 294
#define UMINUS 295
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    1,    2,    2,    2,    3,    3,    4,    9,
    9,   10,   10,   11,   11,   12,   12,    6,    6,    5,
    5,   14,   14,   14,   14,   14,   14,   17,   17,   17,
   17,   17,   17,   17,   18,   18,   18,   18,   18,   18,
   18,   18,   18,   18,   18,   18,   21,   21,   22,   22,
   22,   22,   23,   23,   23,   23,   23,   23,   28,   28,
   28,   24,   19,   19,   20,   20,   30,   30,   16,   16,
   31,   31,   32,   32,   32,   26,    8,    8,   34,   34,
   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,
   25,   13,   13,   27,   27,   29,   29,   29,   15,   15,
   33,   33,   33,   35,   35,   35,   35,   39,   39,   36,
   36,   36,   37,   37,   37,   40,   38,   38,   38,    7,
};
short yylen[] = {                                         2,
    1,    6,    1,    0,    1,    1,    1,    1,    4,    2,
    4,    0,    2,    0,    3,    0,    2,    0,    3,    0,
    3,    3,    3,    2,    3,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    4,    3,    4,    3,
    6,    5,    4,    4,    4,    4,    4,    4,    1,    1,
    1,    2,    6,    5,    5,    4,    0,    2,    1,    3,
    2,    2,    0,    1,    1,    6,    1,    3,    1,    1,
    3,    3,    3,    3,    3,    2,    2,    1,    1,    1,
    3,    1,    3,    1,    3,    1,    1,    1,    1,    3,
    1,    3,    5,    4,    5,    5,    4,    1,    3,    1,
    2,    1,    1,    2,    3,    2,    1,    1,    1,    1,
};
short yydefred[] = {                                      0,
  120,    0,    0,    1,    3,    5,    6,    0,  118,   98,
    0,  119,  117,    0,    0,    8,    0,   79,    0,    0,
    0,    0,   88,   89,   77,   90,   96,   97,    0,    0,
    0,   87,   86,    0,  116,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  114,    0,   91,    0,
    0,  112,    0,   92,    0,    0,    0,    0,    0,   78,
    0,    0,    0,   83,   84,    0,    0,    0,    0,  115,
    0,    0,    0,    0,  111,    0,    0,    2,    0,    0,
    0,   27,    0,   26,   28,   29,   30,   31,   32,   33,
   34,    0,    0,    0,    0,    0,    0,  104,  107,  103,
  109,   11,   93,    0,    0,   69,    0,    0,   24,    0,
   62,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    9,  106,
    0,  105,   74,   75,   71,    0,   72,   19,    0,   25,
    0,   23,    0,    0,    0,    0,    0,   50,    0,   48,
   60,   59,   61,    0,    0,   35,    0,    0,   37,    0,
    0,   40,    0,    0,   42,    0,    0,   43,    0,    0,
   45,    0,    0,    0,   99,    0,    0,   70,    0,    0,
    0,   49,    0,    0,    0,   94,   47,   53,   54,   55,
   56,   57,   58,    0,   66,    0,    0,    0,    0,   65,
    0,   52,    0,   68,  100,    0,    0,   51,   95,   76,
};
short yydgoto[] = {                                       3,
    4,    8,   20,   37,   57,   78,    5,   21,   38,   59,
   94,  129,   53,   83,  174,  105,   84,   85,   86,   87,
   88,   89,   90,   91,   92,  111,  185,  157,   23,  195,
  106,  135,   24,   25,   26,   54,   27,   28,   29,   30,
};
short yysindex[] = {                                   -172,
    0, -175,    0,    0,    0,    0,    0,  573,    0,    0,
  -15,    0,    0,  629,  629,    0,  629,    0, -263, -230,
   28,   33,    0,    0,    0,    0,    0,    0,   50,  -36,
 -181,    0,    0,  -37,    0,  -40, -178, -158,  600,  629,
  629,  629,  629,  629,  526,   59,    0,   72,    0,   75,
 -172,    0,   80,    0, -166, -136, -146,  566, -132,    0,
  -14,  -14,  -14,    0,    0,  629, -155,   99,  -28,    0,
 -150, -148,  104,  -52,    0, -199, -135,    0,  566,  111,
  566,    0, -144,    0,    0,    0,    0,    0,    0,    0,
    0,  429, -110, -114,   -7,  110,  116,    0,    0,    0,
    0,    0,    0, -200,  115,    0, -200,  573,    0, -109,
    0,  -25,  491,  566,  566, -235,  629,  121, -210,  464,
  464,  464,  464,  464,  464,  -49, -155,  566,    0,    0,
 -127,    0,    0,    0,    0, -199,    0,    0, -175,    0,
  -89,    0,  629,  131,  -49,   96,  -51,    0, -106,    0,
    0,    0,    0,  607,   33,    0,  111,   33,    0,  111,
   33,    0,  111,   33,    0,  111,   33,    0,  111,   33,
    0,  111,  -92,  130,    0, -144,   72,    0,  573,  105,
  -51,    0,  -92,  629,   10,    0,    0,    0,    0,    0,
    0,    0,    0,  -49,    0, -155, -230,  629,   68,    0,
   33,    0,  -49,    0,    0,  134,   33,    0,    0,    0,
};
short yyrindex[] = {                                      0,
    0,  596,    0,    0,    0,    0,    0,    0,    0,    0,
   18,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   32,   30,    0,    0,    0,    0,    0,    0,    0,   53,
    0,    0,    0,    0,    0,    0,    2,  188,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   88,    0,    1,
    0,    0,  223,    0,  217,    0,   25,    0,   79,    0,
  123,  158,  193,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  398,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   46,    0,  119,    0,    0,    0,    0,
    0,    0,    0,    3,   11,    0,    3,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  596,    0,
  353,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  231,    0,    0,  248,    0,    0,
  255,    0,    0,  273,    0,    0,  290,    0,    0,  294,
    0,    0,  319,  149,    0,   81,  154,    0,    0,    0,
    0,    0,  319,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  325,    0,    0,    0,    0,    0,  328,    0,    0,    0,
};
short yygindex[] = {                                      0,
  125,   38,  -88,  -19,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -58,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  889,  671,   -2,  -17,  -78,   -3,
   45,   76,  -26,  143,    0,  112,    0,    0,  -10,   56,
};
#define YYTABLESIZE 1087
short yytable[] = {                                      51,
  108,   20,   73,   49,   43,   41,   19,   42,   19,   44,
   21,   52,   99,   43,   41,  140,   42,  101,   44,  138,
  109,   19,  112,   52,   18,   55,  143,   43,   35,   80,
   31,    7,   44,  130,   43,   41,   36,   42,  144,   44,
   97,  108,   20,   73,  108,   16,   73,  173,  149,  107,
  202,   21,  113,  203,  145,  141,  142,  108,  101,  101,
  101,  101,  101,   55,  101,   18,  183,  150,  186,  176,
   80,   39,    7,   80,   43,   41,  104,   42,   14,   44,
   17,  133,  134,    1,    6,   47,   16,  102,    7,   45,
  197,   56,   96,  113,  113,  113,  113,  113,    2,  113,
  175,   70,  186,  160,  163,  166,  169,  172,  208,  107,
   48,  203,  114,  115,   58,  204,   19,   71,  101,   14,
   72,   17,   85,   74,  209,   75,   76,  109,  102,  102,
  102,  102,  102,   77,  102,   93,   96,   43,   41,   98,
   42,  100,   44,  101,  102,  108,   43,   41,   15,   42,
  110,   44,  127,  102,  128,  131,  132,   81,  136,  101,
  147,  139,  101,   85,  177,   85,   85,   85,  115,  205,
  181,  187,  194,  196,  210,   73,  179,  206,  199,  200,
  178,   60,  137,    0,    0,  103,    0,   12,    0,   15,
    0,    0,   82,    0,  102,    0,    0,  102,   81,    0,
   81,   81,   81,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  110,    0,    0,  139,
    0,    0,   10,    0,    9,    0,    9,   10,   12,   10,
   36,  114,  115,   82,    0,   82,   82,   82,   46,   50,
    0,   12,   13,   12,   13,    0,    0,   38,    0,    0,
    0,   50,    0,   40,   39,    0,    0,  110,    0,    0,
  110,    0,   40,   10,    0,    0,    0,    0,  108,  108,
  108,   36,   41,  108,  101,  101,  101,    0,    0,  101,
  108,   20,   73,   40,  101,  101,  101,  101,   38,   44,
   21,  101,  108,   46,  101,   39,   80,  101,    7,    0,
    0,  101,  101,  101,  101,  101,  101,  101,  101,  113,
  113,  113,    0,   41,  113,   16,    0,  113,   67,  113,
  113,  113,  113,   40,   64,   16,  113,   63,    0,  113,
   44,    0,  113,    0,   46,    0,  113,  113,  113,  113,
  113,  113,  113,  113,  102,  102,  102,   14,   14,  102,
   17,    0,   22,  184,  102,  102,  102,  102,   14,   67,
   17,  102,  198,    0,  102,   64,    0,  102,   63,    0,
    0,  102,  102,  102,  102,  102,  102,  102,  102,   85,
   85,   85,    0,    0,   85,    0,   40,  101,  101,   85,
   85,   85,   85,   22,    0,   40,   85,   13,  101,   85,
  101,  101,   85,    0,    0,    0,   85,   85,   85,   85,
   85,   85,   85,   85,   81,   81,   81,   15,   15,   81,
    0,    0,  102,  102,   81,   81,   81,   81,   15,    0,
    0,   81,    0,  102,   81,  102,  102,   81,   13,    0,
    0,   81,   81,   81,   81,   81,   81,   81,   81,   82,
   82,   82,    0,    0,   82,   12,   12,   12,    0,   82,
   82,   82,   82,    0,    0,    0,   82,   12,    0,   82,
   43,   41,   82,   42,    0,   44,   82,   82,   82,   82,
   82,   82,   82,   82,  110,  110,  110,   36,   36,  110,
   10,   10,   10,    0,    0,   10,  110,    0,   36,   36,
   36,    0,   10,  154,   38,   38,   14,    0,   15,    0,
   36,   39,   39,    0,    0,   38,   38,   38,    0,    0,
    0,   19,   39,   39,   39,    0,    0,   38,    0,   41,
   41,   49,   43,   41,   39,   42,    0,   44,    0,    0,
   41,   41,   41,    0,    0,    0,   44,   44,    0,    0,
   46,   46,   41,    0,    0,    0,    0,   44,   44,   44,
    0,   46,   46,   46,    0,   17,    0,   68,   14,   44,
   15,    0,    0,   46,    0,   67,   67,    0,    0,    0,
    0,   64,   64,   19,   63,   63,   67,   67,   67,    0,
    0,    0,   64,   64,   64,   63,   63,   63,   67,    0,
    0,    0,    0,    0,   64,   81,    0,   63,   14,   22,
   15,    0,   17,    0,   16,   14,    0,   15,    0,    0,
   22,   22,   22,   19,    0,    0,    0,    0,    0,   82,
   19,    0,   22,    0,    0,    4,   18,    4,    4,   17,
    4,    0,   14,    0,   15,    0,   17,    0,    0,   14,
    0,   15,    0,    4,    0,    0,    0,   19,    0,    4,
    0,    0,    0,   18,   19,   13,   13,   13,   17,    0,
    0,   14,    0,   15,    0,    0,    0,   13,    0,    0,
    0,    0,    0,    0,    0,    0,   19,  116,    0,    0,
  117,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  118,    0,    0,  119,    0,    0,    0,    0,
    0,    0,  120,  121,  122,  123,  124,  125,  126,   40,
    0,    0,    0,  151,  152,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  153,    0,    0,    0,    9,
    0,    0,   10,    0,    0,    0,    0,    0,    0,  116,
    0,    0,  117,    0,    0,   11,   12,   13,    0,    0,
    0,    0,    0,    0,  118,    0,    0,  119,    0,    0,
    0,    0,    0,    0,  120,  121,  122,  123,  124,  125,
  126,   40,    0,    0,    0,   66,    0,    0,  148,   67,
  156,  159,  162,  165,  168,  171,    0,    0,    0,    0,
    0,    9,    0,    0,   10,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  182,    0,    0,   11,   12,   13,
    0,    0,    0,    0,   79,    0,    0,  188,    0,    0,
  189,   80,    0,  190,    0,    0,  191,    0,    0,  192,
    0,    9,  193,    0,   10,    0,    0,    0,    9,    0,
    0,   10,    0,    0,    0,    0,    0,   11,   12,   13,
    0,    0,    0,    0,   11,   12,   13,    0,    0,    0,
    0,    4,    0,    0,    4,    9,    0,  139,   10,    0,
    0,    0,    9,    0,    0,   10,    0,    4,    4,    4,
    0,   11,   12,   13,    0,    0,   22,    0,   11,   12,
   13,    0,   32,   33,    9,   34,    0,   10,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   11,   12,   13,    0,    0,    0,    0,   22,   61,   62,
   63,   64,   65,   69,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   95,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  113,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   22,    0,    0,    0,
    0,    0,    0,    0,    0,  146,    0,    0,  155,  158,
  161,  164,  167,  170,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  180,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   34,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   22,    0,    0,
    0,    0,  201,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  207,
};
short yycheck[] = {                                      40,
    0,    0,    0,   41,   42,   43,   58,   45,   58,   47,
    0,   64,   41,   42,   43,   41,   45,    0,   47,  108,
   79,   58,   81,   64,    0,   36,  262,   42,  292,    0,
   46,    0,   47,   41,   42,   43,  267,   45,  274,   47,
   67,   41,   41,   41,   44,    0,   44,  126,  259,   76,
   41,   41,    0,   44,  290,  114,  115,   40,   41,   42,
   43,   44,   45,   74,   47,   41,  145,  278,  147,  128,
   41,   44,   41,   44,   42,   43,  276,   45,    0,   47,
    0,  282,  283,  256,  260,   30,   41,    0,  264,   40,
  179,  270,  292,   41,   42,   43,   44,   45,  271,   47,
  127,   46,  181,  121,  122,  123,  124,  125,   41,  136,
  292,   44,  257,  258,  273,  194,   58,   46,    0,   41,
   46,   41,    0,   44,  203,  292,  263,   40,   41,   42,
   43,   44,   45,  280,   47,  268,  292,   42,   43,   41,
   45,  292,   47,  292,   41,  281,   42,   43,    0,   45,
   40,   47,  263,    0,  269,   46,   41,    0,   44,   41,
   40,  271,   44,   41,  292,   43,   44,   45,  258,  196,
   40,  278,  265,   44,   41,   51,  139,  197,  181,  183,
  136,   39,  107,   -1,   -1,   74,   -1,    0,   -1,   41,
   -1,   -1,    0,   -1,   41,   -1,   -1,   44,   41,   -1,
   43,   44,   45,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,   -1,  271,
   -1,   -1,    0,   -1,  276,   -1,  276,  279,   41,  279,
    0,  257,  258,   41,   -1,   43,   44,   45,  275,  292,
   -1,  293,  294,  293,  294,   -1,   -1,    0,   -1,   -1,
   -1,  292,   -1,  291,    0,   -1,   -1,   41,   -1,   -1,
   44,   -1,  291,   41,   -1,   -1,   -1,   -1,  268,  269,
  270,   41,    0,  273,  257,  258,  259,   -1,   -1,  262,
  280,  280,  280,  291,  267,  268,  269,  270,   41,    0,
  280,  274,  292,    0,  277,   41,  267,  280,  267,   -1,
   -1,  284,  285,  286,  287,  288,  289,  290,  291,  257,
  258,  259,   -1,   41,  262,  270,   -1,  265,    0,  267,
  268,  269,  270,  291,    0,  280,  274,    0,   -1,  277,
   41,   -1,  280,   -1,   41,   -1,  284,  285,  286,  287,
  288,  289,  290,  291,  257,  258,  259,  269,  270,  262,
  270,   -1,    0,  258,  267,  268,  269,  270,  280,   41,
  280,  274,  258,   -1,  277,   41,   -1,  280,   41,   -1,
   -1,  284,  285,  286,  287,  288,  289,  290,  291,  257,
  258,  259,   -1,   -1,  262,   -1,  291,  269,  270,  267,
  268,  269,  270,   41,   -1,  291,  274,    0,  280,  277,
  282,  283,  280,   -1,   -1,   -1,  284,  285,  286,  287,
  288,  289,  290,  291,  257,  258,  259,  269,  270,  262,
   -1,   -1,  269,  270,  267,  268,  269,  270,  280,   -1,
   -1,  274,   -1,  280,  277,  282,  283,  280,   41,   -1,
   -1,  284,  285,  286,  287,  288,  289,  290,  291,  257,
  258,  259,   -1,   -1,  262,  268,  269,  270,   -1,  267,
  268,  269,  270,   -1,   -1,   -1,  274,  280,   -1,  277,
   42,   43,  280,   45,   -1,   47,  284,  285,  286,  287,
  288,  289,  290,  291,  268,  269,  270,  257,  258,  273,
  268,  269,  270,   -1,   -1,  273,  280,   -1,  268,  269,
  270,   -1,  280,   40,  257,  258,   43,   -1,   45,   -1,
  280,  257,  258,   -1,   -1,  268,  269,  270,   -1,   -1,
   -1,   58,  268,  269,  270,   -1,   -1,  280,   -1,  257,
  258,   41,   42,   43,  280,   45,   -1,   47,   -1,   -1,
  268,  269,  270,   -1,   -1,   -1,  257,  258,   -1,   -1,
  257,  258,  280,   -1,   -1,   -1,   -1,  268,  269,  270,
   -1,  268,  269,  270,   -1,   40,   -1,   42,   43,  280,
   45,   -1,   -1,  280,   -1,  257,  258,   -1,   -1,   -1,
   -1,  257,  258,   58,  257,  258,  268,  269,  270,   -1,
   -1,   -1,  268,  269,  270,  268,  269,  270,  280,   -1,
   -1,   -1,   -1,   -1,  280,   40,   -1,  280,   43,  257,
   45,   -1,   40,   -1,   42,   43,   -1,   45,   -1,   -1,
  268,  269,  270,   58,   -1,   -1,   -1,   -1,   -1,   64,
   58,   -1,  280,   -1,   -1,   40,   64,   42,   43,   40,
   45,   -1,   43,   -1,   45,   -1,   40,   -1,   -1,   43,
   -1,   45,   -1,   58,   -1,   -1,   -1,   58,   -1,   64,
   -1,   -1,   -1,   64,   58,  268,  269,  270,   40,   -1,
   -1,   43,   -1,   45,   -1,   -1,   -1,  280,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   58,  259,   -1,   -1,
  262,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  274,   -1,   -1,  277,   -1,   -1,   -1,   -1,
   -1,   -1,  284,  285,  286,  287,  288,  289,  290,  291,
   -1,   -1,   -1,  260,  261,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  272,   -1,   -1,   -1,  276,
   -1,   -1,  279,   -1,   -1,   -1,   -1,   -1,   -1,  259,
   -1,   -1,  262,   -1,   -1,  292,  293,  294,   -1,   -1,
   -1,   -1,   -1,   -1,  274,   -1,   -1,  277,   -1,   -1,
   -1,   -1,   -1,   -1,  284,  285,  286,  287,  288,  289,
  290,  291,   -1,   -1,   -1,  260,   -1,   -1,  118,  264,
  120,  121,  122,  123,  124,  125,   -1,   -1,   -1,   -1,
   -1,  276,   -1,   -1,  279,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  144,   -1,   -1,  292,  293,  294,
   -1,   -1,   -1,   -1,  259,   -1,   -1,  157,   -1,   -1,
  160,  266,   -1,  163,   -1,   -1,  166,   -1,   -1,  169,
   -1,  276,  172,   -1,  279,   -1,   -1,   -1,  276,   -1,
   -1,  279,   -1,   -1,   -1,   -1,   -1,  292,  293,  294,
   -1,   -1,   -1,   -1,  292,  293,  294,   -1,   -1,   -1,
   -1,  276,   -1,   -1,  279,  276,   -1,  271,  279,   -1,
   -1,   -1,  276,   -1,   -1,  279,   -1,  292,  293,  294,
   -1,  292,  293,  294,   -1,   -1,    8,   -1,  292,  293,
  294,   -1,   14,   15,  276,   17,   -1,  279,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  292,  293,  294,   -1,   -1,   -1,   -1,   39,   40,   41,
   42,   43,   44,   45,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   66,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   81,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  108,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  117,   -1,   -1,  120,  121,
  122,  123,  124,  125,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  143,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  154,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  179,   -1,   -1,
   -1,   -1,  184,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  198,
};
#define YYFINAL 3
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 295
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,0,0,0,0,0,0,0,0,
"':'",0,0,0,0,0,"'@'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"OR","AND","NOT","ALL","ANY","BETWEEN","BY","DISTINCT","ESCAPE","EXISTS",
"FROM","GROUP","HAVING","ORDER","SELECT","SOME","WHERE","IN","INDICATOR",
"INTNUM","IS","DBNULL","USER","WITH","KEY","ASC","DESC","LESS","LESS_EQ","GT",
"GT_EQ","EQ","NEQ","LIKE","CONCAT_OP","IDENT","APPROXNUM","STRING","UMINUS",
};
char *yyrule[] = {
"$accept : startTerm",
"startTerm : query",
"query : SELECT select_opt selection table_expr opt_order_by_clause opt_key_clause",
"query : was_error",
"select_opt :",
"select_opt : ALL",
"select_opt : DISTINCT",
"selection : select_scalar_expr_comma_list",
"selection : '*'",
"table_expr : from_clause opt_where_clause opt_group_by_clause opt_having_clause",
"from_clause : FROM table_ref_commalist",
"from_clause : FROM '(' query ')'",
"opt_where_clause :",
"opt_where_clause : WHERE search_condition",
"opt_group_by_clause :",
"opt_group_by_clause : GROUP BY column_ref_commalist",
"opt_having_clause :",
"opt_having_clause : HAVING search_condition",
"opt_key_clause :",
"opt_key_clause : WITH KEY selection",
"opt_order_by_clause :",
"opt_order_by_clause : ORDER BY ordering_spec_commalist",
"search_condition : search_condition OR search_condition",
"search_condition : search_condition AND search_condition",
"search_condition : NOT search_condition",
"search_condition : '(' search_condition ')'",
"search_condition : predicate",
"search_condition : '@'",
"predicate : comparison_predicate",
"predicate : between_predicate",
"predicate : like_predicate",
"predicate : test_for_null",
"predicate : in_predicate",
"predicate : all_or_any_predicate",
"predicate : existence_predicate",
"comparison_predicate : scalar_expr LESS subquery",
"comparison_predicate : scalar_expr LESS scalar_expr",
"comparison_predicate : scalar_expr LESS_EQ subquery",
"comparison_predicate : scalar_expr LESS_EQ scalar_expr",
"comparison_predicate : scalar_expr GT scalar_expr",
"comparison_predicate : scalar_expr GT subquery",
"comparison_predicate : scalar_expr GT_EQ scalar_expr",
"comparison_predicate : scalar_expr GT_EQ subquery",
"comparison_predicate : scalar_expr EQ subquery",
"comparison_predicate : scalar_expr EQ scalar_expr",
"comparison_predicate : scalar_expr NEQ subquery",
"comparison_predicate : scalar_expr NEQ scalar_expr",
"test_for_null : scalar_expr IS NOT DBNULL",
"test_for_null : scalar_expr IS DBNULL",
"in_predicate : scalar_expr NOT IN subquery",
"in_predicate : scalar_expr IN subquery",
"in_predicate : scalar_expr NOT IN '(' atom_commalist ')'",
"in_predicate : scalar_expr IN '(' atom_commalist ')'",
"all_or_any_predicate : scalar_expr LESS any_all_some subquery",
"all_or_any_predicate : scalar_expr LESS_EQ any_all_some subquery",
"all_or_any_predicate : scalar_expr GT any_all_some subquery",
"all_or_any_predicate : scalar_expr GT_EQ any_all_some subquery",
"all_or_any_predicate : scalar_expr EQ any_all_some subquery",
"all_or_any_predicate : scalar_expr NEQ any_all_some subquery",
"any_all_some : ANY",
"any_all_some : ALL",
"any_all_some : SOME",
"existence_predicate : EXISTS subquery",
"between_predicate : scalar_expr NOT BETWEEN scalar_expr AND scalar_expr",
"between_predicate : scalar_expr BETWEEN scalar_expr AND scalar_expr",
"like_predicate : scalar_expr NOT LIKE atom opt_escape",
"like_predicate : scalar_expr LIKE atom opt_escape",
"opt_escape :",
"opt_escape : ESCAPE atom",
"ordering_spec_commalist : ordering_spec",
"ordering_spec_commalist : ordering_spec_commalist ',' ordering_spec",
"ordering_spec : INTNUM opt_ordering",
"ordering_spec : column_ref opt_ordering",
"opt_ordering :",
"opt_ordering : ASC",
"opt_ordering : DESC",
"subquery : '(' SELECT select_opt selection table_expr ')'",
"select_scalar_expr_comma_list : select_scalar_expr",
"select_scalar_expr_comma_list : select_scalar_expr_comma_list ',' select_scalar_expr",
"select_scalar_expr : '@'",
"select_scalar_expr : scalar_expr",
"scalar_expr : scalar_expr '+' scalar_expr",
"scalar_expr : scalar_expr '-' scalar_expr",
"scalar_expr : scalar_expr '*' scalar_expr",
"scalar_expr : scalar_expr '/' scalar_expr",
"scalar_expr : scalar_expr CONCAT_OP scalar_expr",
"scalar_expr : '-' scalar_expr",
"scalar_expr : '+' scalar_expr",
"scalar_expr : atom",
"scalar_expr : column_ref",
"scalar_expr : function_ref",
"scalar_expr : '(' scalar_expr ')'",
"table_ref_commalist : table_ref",
"table_ref_commalist : table_ref_commalist ',' table_ref",
"atom_commalist : atom",
"atom_commalist : atom_commalist ',' atom",
"atom : parameter_ref",
"atom : literal",
"atom : USER",
"column_ref_commalist : column_ref",
"column_ref_commalist : column_ref_commalist ',' column_ref",
"column_ref : IDENT",
"column_ref : IDENT '.' IDENT",
"column_ref : IDENT '.' IDENT '.' IDENT",
"function_ref : compound_name '(' '*' ')'",
"function_ref : compound_name '(' DISTINCT column_ref ')'",
"function_ref : compound_name '(' ALL scalar_expr ')'",
"function_ref : compound_name '(' scalar_expr ')'",
"compound_name : IDENT",
"compound_name : IDENT '.' IDENT",
"table_ref : compound_name",
"table_ref : compound_name IDENT",
"table_ref : '@'",
"parameter_ref : parameter",
"parameter_ref : parameter parameter",
"parameter_ref : parameter INDICATOR parameter",
"parameter : ':' IDENT",
"literal : STRING",
"literal : INTNUM",
"literal : APPROXNUM",
"was_error : error",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 556 "query.y"

int yyerror(char *s) {
    yy_setParseError(s);
    yyval=NULL;
    return 0;
}
#line 587 "y.tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 94 "query.y"
{ yyval =  yyvsp[0]; }
break;
case 2:
#line 99 "query.y"
{ yyval =  yy_createSelectSQLTerm(yyvsp[-4], yyvsp[-3], yyvsp[-2], yyvsp[-1], yyvsp[0]); }
break;
case 3:
#line 102 "query.y"
{ yyval = yy_createConstStringTerm("{ERROR}"); YYABORT; }
break;
case 4:
#line 107 "query.y"
{ yyval =  yy_createSelectOptTerm(1); }
break;
case 5:
#line 110 "query.y"
{ yyval =  yy_createSelectOptTerm(1); }
break;
case 6:
#line 113 "query.y"
{ yyval =  yy_createSelectOptTerm(0); }
break;
case 7:
#line 118 "query.y"
{ yyval = yyvsp[0]; }
break;
case 8:
#line 121 "query.y"
{ yyval = yy_createConstStringTerm("*"); }
break;
case 9:
#line 130 "query.y"
{
          yyval=yy_createTableExprSQLTerm(yyvsp[-3], yyvsp[-2], yyvsp[-1], yyvsp[0]);
        }
break;
case 10:
#line 137 "query.y"
{
            yyval=yyvsp[0];
          }
break;
case 11:
#line 142 "query.y"
{
            yyval=yy_catStringTerm3_ctc(" (", yyvsp[-1], ")" );
          }
break;
case 12:
#line 149 "query.y"
{ yyval=NULL; }
break;
case 13:
#line 152 "query.y"
{ yyval=yyvsp[0]; }
break;
case 14:
#line 157 "query.y"
{   yyval=NULL; }
break;
case 15:
#line 160 "query.y"
{ 
             yy_setParseError("GROUP by not supported in collection query"); 
             yyval=yyvsp[0];
          }
break;
case 16:
#line 168 "query.y"
{  yyval=NULL; }
break;
case 17:
#line 171 "query.y"
{  yy_setParseError("HAVING not supported in collection query"); 
             yyval=yyvsp[0];
          }
break;
case 18:
#line 178 "query.y"
{ yyval=NULL; }
break;
case 19:
#line 181 "query.y"
{ yyval=yyvsp[0]; }
break;
case 20:
#line 187 "query.y"
{   yyval=NULL; }
break;
case 21:
#line 190 "query.y"
{  yyval=yyvsp[0]; }
break;
case 22:
#line 196 "query.y"
{ yyval = yy_catStringTerm3_tct(yyvsp[-2],"OR",yyvsp[0]); }
break;
case 23:
#line 199 "query.y"
{ yyval = yy_catStringTerm3_tct(yyvsp[-2],"AND",yyvsp[0]); }
break;
case 24:
#line 202 "query.y"
{ yyval = yy_catStringTerm2_ct("NOT",yyvsp[0]); }
break;
case 25:
#line 205 "query.y"
{ yyval = yy_catStringTerm3_ctc("(",yyvsp[-1],")"); }
break;
case 26:
#line 208 "query.y"
{ yyval = yyvsp[0]; }
break;
case 27:
#line 211 "query.y"
{ yyval = yy_createConstStringTerm("@"); }
break;
case 28:
#line 216 "query.y"
{ yyval=yyvsp[0]; }
break;
case 29:
#line 219 "query.y"
{ yyval=yyvsp[0]; }
break;
case 30:
#line 222 "query.y"
{ yyval=yyvsp[0]; }
break;
case 31:
#line 225 "query.y"
{ yyval=yyvsp[0]; }
break;
case 32:
#line 228 "query.y"
{ yyval=yyvsp[0]; }
break;
case 33:
#line 231 "query.y"
{ yyval=yyvsp[0]; }
break;
case 34:
#line 234 "query.y"
{ yyval=yyvsp[0]; }
break;
case 35:
#line 239 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"<",yyvsp[0]); }
break;
case 36:
#line 242 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"<", yyvsp[0]); }
break;
case 37:
#line 245 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"<=",yyvsp[0]); }
break;
case 38:
#line 248 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"<=", yyvsp[0]); }
break;
case 39:
#line 251 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],">",yyvsp[0]); }
break;
case 40:
#line 254 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],">",yyvsp[0]); }
break;
case 41:
#line 257 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],">=",yyvsp[0]); }
break;
case 42:
#line 260 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],">=",yyvsp[0]); }
break;
case 43:
#line 263 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"=",yyvsp[0]); }
break;
case 44:
#line 266 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"=",yyvsp[0]); }
break;
case 45:
#line 269 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"<>",yyvsp[0]); }
break;
case 46:
#line 272 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"<>",yyvsp[0]); }
break;
case 47:
#line 277 "query.y"
{ yyval=yy_catStringTerm2_tc(yyvsp[-3],"IS NOT NULL"); }
break;
case 48:
#line 280 "query.y"
{ yyval=yy_catStringTerm2_tc(yyvsp[-2], "IS NULL"); }
break;
case 49:
#line 286 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-3], "NOT IN", yyvsp[-1]); }
break;
case 50:
#line 289 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2], "IN", yyvsp[0]); }
break;
case 51:
#line 292 "query.y"
{ yyval=yy_catStringTerm4_tctc(yyvsp[-5], "NOT IN (", yyvsp[-1], ")"); }
break;
case 52:
#line 295 "query.y"
{ yyval=yy_catStringTerm4_tctc(yyvsp[-4], "IN (", yyvsp[0], ")"); }
break;
case 53:
#line 300 "query.y"
{ yyval=yy_catStringTerm4_tctt(yyvsp[-3],"<",yyvsp[-1],yyvsp[0]); }
break;
case 54:
#line 303 "query.y"
{ yyval=yy_catStringTerm4_tctt(yyvsp[-3],"<=",yyvsp[-1],yyvsp[0]); }
break;
case 55:
#line 306 "query.y"
{ yyval=yy_catStringTerm4_tctt(yyvsp[-3],">",yyvsp[-1],yyvsp[0]); }
break;
case 56:
#line 309 "query.y"
{ yyval=yy_catStringTerm4_tctt(yyvsp[-3],">=",yyvsp[-1],yyvsp[0]); }
break;
case 57:
#line 312 "query.y"
{ yyval=yy_catStringTerm4_tctt(yyvsp[-3],"=",yyvsp[-1],yyvsp[0]); }
break;
case 58:
#line 315 "query.y"
{ yyval=yy_catStringTerm4_tctt(yyvsp[-3],"<>",yyvsp[-1],yyvsp[0]); }
break;
case 59:
#line 320 "query.y"
{ yyval=yy_createConstStringTerm("ANY"); }
break;
case 60:
#line 323 "query.y"
{ yyval=yy_createConstStringTerm("ALL"); }
break;
case 61:
#line 326 "query.y"
{ yyval=yy_createConstStringTerm("SOME"); }
break;
case 62:
#line 331 "query.y"
{ yyval=yy_catStringTerm2_ct("EXISTS",yyvsp[0]); }
break;
case 63:
#line 336 "query.y"
{ yyval=yy_catStringTerm5_tctct(yyvsp[-5],"NOT BETWEEN",yyvsp[-2],"AND",yyvsp[0]); }
break;
case 64:
#line 339 "query.y"
{ yyval=yy_catStringTerm5_tctct(yyvsp[-4],"BETWEEN",yyvsp[-2],"AND",yyvsp[0]); }
break;
case 65:
#line 344 "query.y"
{ yyval=yy_catStringTerm4_tctt(yyvsp[-4],"NOT LIKE",yyvsp[-2],yyvsp[-1]); }
break;
case 66:
#line 347 "query.y"
{ yyval=yy_catStringTerm4_tctt(yyvsp[-3],"LIKE",yyvsp[-2],yyvsp[-1]); }
break;
case 67:
#line 352 "query.y"
{ yyval=NULL; }
break;
case 68:
#line 355 "query.y"
{ yyval=yy_catStringTerm2_ct("ESCAPE",yyvsp[0]); }
break;
case 69:
#line 360 "query.y"
{ yyval=yyvsp[0]; }
break;
case 70:
#line 363 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],",",yyvsp[0]); }
break;
case 71:
#line 368 "query.y"
{ yyval=yy_catStringTerm2_tt(yyvsp[-1],yyvsp[0]); }
break;
case 72:
#line 371 "query.y"
{ yyval=yy_catStringTerm2_tt(yyvsp[-1],yyvsp[0]); }
break;
case 73:
#line 376 "query.y"
{ yyval=yy_createConstStringTerm("ASC"); }
break;
case 74:
#line 379 "query.y"
{ yyval=yy_createConstStringTerm("ASC");  }
break;
case 75:
#line 382 "query.y"
{ yyval=yy_createConstStringTerm("DESC"); }
break;
case 76:
#line 387 "query.y"
{ yyval=yy_catStringTerm6_cttttc("(",yyvsp[-4],yyvsp[-3],yyvsp[-2],yyvsp[-1],")");  }
break;
case 77:
#line 392 "query.y"
{ yyval=yyvsp[0]; }
break;
case 78:
#line 395 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],",",yyvsp[0]); }
break;
case 79:
#line 400 "query.y"
{ yyval = yy_createConstStringTerm("@"); }
break;
case 80:
#line 403 "query.y"
{ yyval=yyvsp[0]; }
break;
case 81:
#line 410 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"+",yyvsp[0]); }
break;
case 82:
#line 413 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"-",yyvsp[0]); }
break;
case 83:
#line 416 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"*",yyvsp[0]); }
break;
case 84:
#line 419 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"/",yyvsp[0]); }
break;
case 85:
#line 422 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"||",yyvsp[0]); }
break;
case 86:
#line 425 "query.y"
{ yyval=yy_catStringTerm2_ct("+",yyvsp[0]); }
break;
case 87:
#line 428 "query.y"
{ yyval=yy_catStringTerm2_ct("+",yyvsp[0]); }
break;
case 88:
#line 431 "query.y"
{ yyval=yyvsp[0]; }
break;
case 89:
#line 434 "query.y"
{ yyval=yyvsp[0]; }
break;
case 90:
#line 437 "query.y"
{ yyval=yyvsp[0]; }
break;
case 91:
#line 440 "query.y"
{ yyval=yy_catStringTerm3_ctc("(",yyvsp[-1],")"); }
break;
case 92:
#line 445 "query.y"
{ yyval=yyvsp[0]; }
break;
case 93:
#line 448 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],",",yyvsp[0]); }
break;
case 94:
#line 453 "query.y"
{ yyval=yyvsp[0]; }
break;
case 95:
#line 456 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],",",yyvsp[0]); }
break;
case 96:
#line 461 "query.y"
{ yyval=yyvsp[0]; }
break;
case 97:
#line 464 "query.y"
{ yyval=yyvsp[0]; }
break;
case 98:
#line 467 "query.y"
{ yyval=yy_createConstStringTerm("USER"); }
break;
case 99:
#line 472 "query.y"
{ yyval=yyvsp[0]; }
break;
case 100:
#line 475 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],",",yyvsp[0]); }
break;
case 101:
#line 480 "query.y"
{ yyval=yyvsp[0]; }
break;
case 102:
#line 483 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],".",yyvsp[0]); }
break;
case 103:
#line 486 "query.y"
{ yyval=yy_catStringTerm5_tctct(yyvsp[-4],".",yyvsp[-2],".",yyvsp[0]); }
break;
case 104:
#line 491 "query.y"
{ yyval=yy_catStringTerm2_tc(yyvsp[-3],"(*)"); }
break;
case 105:
#line 494 "query.y"
{ yyval=yy_catStringTerm4_tctc(yyvsp[-4],"( DISTINCT",yyvsp[-2],")"); }
break;
case 106:
#line 497 "query.y"
{ yyval=yy_catStringTerm4_tctc(yyvsp[-4],"( ALL",yyvsp[-2],")"); }
break;
case 107:
#line 500 "query.y"
{ yyval=yy_catStringTerm4_tctc(yyvsp[-3],"(",yyvsp[-1],")"); }
break;
case 108:
#line 505 "query.y"
{ yyval=yyvsp[0]; }
break;
case 109:
#line 508 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],".",yyvsp[0]); }
break;
case 110:
#line 513 "query.y"
{ yyval=yyvsp[0]; }
break;
case 111:
#line 516 "query.y"
{ yyval=yy_catStringTerm2_tt(yyvsp[-1],yyvsp[0]); }
break;
case 112:
#line 519 "query.y"
{ yyval = yy_createConstStringTerm("@"); }
break;
case 113:
#line 524 "query.y"
{ yyval=yyvsp[0]; }
break;
case 114:
#line 527 "query.y"
{ yyval=yy_catStringTerm2_tt(yyvsp[-1],yyvsp[0]); }
break;
case 115:
#line 530 "query.y"
{ yyval=yy_catStringTerm3_tct(yyvsp[-2],"INDICATOR",yyvsp[0]); }
break;
case 116:
#line 536 "query.y"
{ yyval=yy_catStringTerm2_ct(":",yyvsp[0]); }
break;
case 117:
#line 541 "query.y"
{ yyval=yyvsp[0]; }
break;
case 118:
#line 544 "query.y"
{ yyval=yyvsp[0]; }
break;
case 119:
#line 547 "query.y"
{ yyval=yyvsp[0]; }
break;
case 120:
#line 552 "query.y"
{ yyval=NULL; }
break;
#line 1219 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
