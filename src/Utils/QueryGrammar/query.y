
%{
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


%}

/*
%pure_parser
*/

%token OR
%token AND
%token NOT

%token ALL
%token ANY
%token BETWEEN
%token BY
%token DISTINCT
%token ESCAPE
%token EXISTS
%token FROM
%token GROUP
%token HAVING
%token ORDER
%token SELECT
%token SOME
%token WHERE 
%token IN 
%token INDICATOR 
%token INTNUM 
%token IS 
%token DBNULL
%token USER
%token WITH
%token KEY 

%token ASC
%token DESC

%token LESS
%token LESS_EQ
%token GT
%token GT_EQ
%token EQ
%token NEQ
%token LIKE
%token CONCAT_OP

%token IDENT
%token INTNUM
%token APPROXNUM
%token STRING

%left OR
%left AND
%left NOT
%left LESS LESS_EQ GT GT_EQ EQ NEQ
%left '+' '-' CONCAT_OP
%left '*' '/'
%nonassoc UMINUS



%start startTerm

%%

startTerm: query
           { $$ =  $1; } 
          ;

query:
	SELECT select_opt selection table_expr opt_order_by_clause opt_key_clause
       { $$ =  yy_createSelectSQLTerm($2, $3, $4, $5, $6); } 
        |
         was_error
        { $$ = yy_createConstStringTerm("{ERROR}"); YYABORT; }
       ;

select_opt:
          /* empty */
          { $$ =  yy_createSelectOptTerm(1); }
	|
	 ALL
          { $$ =  yy_createSelectOptTerm(1); }
	|
         DISTINCT
          { $$ =  yy_createSelectOptTerm(0); }
        ;

selection:
	select_scalar_expr_comma_list
          { $$ = $1; }
	|
	'*'
          { $$ = yy_createConstStringTerm("*"); }
        ;


table_expr:
        from_clause
          opt_where_clause 
            opt_group_by_clause 
             opt_having_clause  
        {
          $$=yy_createTableExprSQLTerm($1, $2, $3, $4);
        }
           ;

from_clause:
         FROM table_ref_commalist
          {
            $$=$2;
          }
         |
         FROM '(' query ')' 
          {
            $$=yy_catStringTerm3_ctc(" (", $3, ")" );
          }
         ;

opt_where_clause:
              /* empty */
               { $$=NULL; }
             |
              WHERE search_condition
               { $$=$2; }
             ;

opt_group_by_clause:
                  /* empty */
                {   $$=NULL; }
                 |
                  GROUP BY column_ref_commalist
          { 
             yy_setParseError("GROUP by not supported in collection query"); 
             $$=$3;
          }
                 ;
 
opt_having_clause:
                   /* empty */
                {  $$=NULL; }
                  |
                   HAVING search_condition
          {  yy_setParseError("HAVING not supported in collection query"); 
             $$=$2;
          }
                  ;

opt_key_clause:
                /* empty */
               { $$=NULL; }
                |
                 WITH KEY selection
                { $$=$3; }
                ;


opt_order_by_clause:
                  /* empty */
                {   $$=NULL; }
                  |
                   ORDER BY ordering_spec_commalist
                {  $$=$3; }
                  ;


search_condition:
                  search_condition OR search_condition
                   { $$ = yy_catStringTerm3_tct($1,"OR",$3); }
                 |
                  search_condition AND search_condition
                   { $$ = yy_catStringTerm3_tct($1,"AND",$3); }
                 |
                  NOT search_condition
                   { $$ = yy_catStringTerm2_ct("NOT",$2); }
                 |
                  '(' search_condition ')'
                   { $$ = yy_catStringTerm3_ctc("(",$2,")"); }
                 |
                  predicate
                   { $$ = $1; }
                 |
                  '@'
                    { $$ = yy_createConstStringTerm("@"); }
                 ;

predicate:
           comparison_predicate
            { $$=$1; }
          |
           between_predicate
            { $$=$1; }
          |
           like_predicate
            { $$=$1; }
          |
           test_for_null
            { $$=$1; }
          |
           in_predicate
            { $$=$1; }
          |
           all_or_any_predicate
            { $$=$1; }
          |
           existence_predicate
            { $$=$1; }
          ;

comparison_predicate:
	           scalar_expr LESS  subquery
                     { $$=yy_catStringTerm3_tct($1,"<",$3); }
                  |
                   scalar_expr LESS  scalar_expr
                     { $$=yy_catStringTerm3_tct($1,"<", $3); }
                  |
	           scalar_expr LESS_EQ  subquery
                     { $$=yy_catStringTerm3_tct($1,"<=",$3); }
                  |
                   scalar_expr LESS_EQ  scalar_expr
                     { $$=yy_catStringTerm3_tct($1,"<=", $3); }
                  |
                   scalar_expr GT  scalar_expr
                     { $$=yy_catStringTerm3_tct($1,">",$3); }
                  |
                   scalar_expr GT  subquery
                     { $$=yy_catStringTerm3_tct($1,">",$3); }
                  |
                   scalar_expr GT_EQ  scalar_expr
                     { $$=yy_catStringTerm3_tct($1,">=",$3); }
                  |
                   scalar_expr GT_EQ  subquery
                     { $$=yy_catStringTerm3_tct($1,">=",$3); }
                  |
                   scalar_expr EQ subquery  
                     { $$=yy_catStringTerm3_tct($1,"=",$3); }
                  |
                   scalar_expr EQ scalar_expr 
                     { $$=yy_catStringTerm3_tct($1,"=",$3); }
                  |
                   scalar_expr NEQ subquery
                     { $$=yy_catStringTerm3_tct($1,"<>",$3); }
                  | 
                   scalar_expr NEQ scalar_expr
                     { $$=yy_catStringTerm3_tct($1,"<>",$3); }
                  ;
                   
test_for_null:
	scalar_expr IS NOT DBNULL
          { $$=yy_catStringTerm2_tc($1,"IS NOT NULL"); }
        |
        scalar_expr IS DBNULL
          { $$=yy_catStringTerm2_tc($1, "IS NULL"); }
        ;
 

in_predicate:
        scalar_expr NOT IN  subquery 
          { $$=yy_catStringTerm3_tct($1, "NOT IN", $3); }
       |
        scalar_expr IN  subquery 
          { $$=yy_catStringTerm3_tct($1, "IN", $3); }
       |
        scalar_expr NOT IN '(' atom_commalist ')'
          { $$=yy_catStringTerm4_tctc($1, "NOT IN (", $5, ")"); }
       |
        scalar_expr IN '(' atom_commalist ')'
          { $$=yy_catStringTerm4_tctc($1, "IN (", $5, ")"); }
       ;

all_or_any_predicate:
            scalar_expr LESS any_all_some subquery
           { $$=yy_catStringTerm4_tctt($1,"<",$3,$4); }
           |
            scalar_expr LESS_EQ any_all_some subquery
           { $$=yy_catStringTerm4_tctt($1,"<=",$3,$4); }
           |
            scalar_expr GT any_all_some subquery
           { $$=yy_catStringTerm4_tctt($1,">",$3,$4); }
           | 
            scalar_expr GT_EQ any_all_some subquery
           { $$=yy_catStringTerm4_tctt($1,">=",$3,$4); }
           |
            scalar_expr EQ any_all_some subquery
           { $$=yy_catStringTerm4_tctt($1,"=",$3,$4); }
           |
            scalar_expr NEQ any_all_some subquery
           { $$=yy_catStringTerm4_tctt($1,"<>",$3,$4); }
           ;

any_all_some:
           ANY 
            { $$=yy_createConstStringTerm("ANY"); }
          |
           ALL
            { $$=yy_createConstStringTerm("ALL"); }
          |
           SOME
            { $$=yy_createConstStringTerm("SOME"); }
          ;

existence_predicate:
            EXISTS subquery
           { $$=yy_catStringTerm2_ct("EXISTS",$2); }
           ;

between_predicate:
                   scalar_expr NOT BETWEEN scalar_expr AND scalar_expr
           { $$=yy_catStringTerm5_tctct($1,"NOT BETWEEN",$4,"AND",$6); }
                  |
                   scalar_expr BETWEEN scalar_expr AND scalar_expr
           { $$=yy_catStringTerm5_tctct($1,"BETWEEN",$3,"AND",$5); }
                  ;

like_predicate:
	        scalar_expr NOT LIKE atom opt_escape
           { $$=yy_catStringTerm4_tctt($1,"NOT LIKE",$3,$4); }
               |
                scalar_expr LIKE atom opt_escape
           { $$=yy_catStringTerm4_tctt($1,"LIKE",$2,$3); }
               ;        
 
opt_escape:
	  /* empty */
           { $$=NULL; }
         |
          ESCAPE atom
           { $$=yy_catStringTerm2_ct("ESCAPE",$2); }
         ;

ordering_spec_commalist:
         ordering_spec
           { $$=$1; }
         |
         ordering_spec_commalist ',' ordering_spec
           { $$=yy_catStringTerm3_tct($1,",",$3); }
         ;

ordering_spec:
           INTNUM opt_ordering
           { $$=yy_catStringTerm2_tt($1,$2); }
          |
           column_ref opt_ordering
           { $$=yy_catStringTerm2_tt($1,$2); }
          ;

opt_ordering:
         /* empty */
           { $$=yy_createConstStringTerm("ASC"); }
         |
          ASC
           { $$=yy_createConstStringTerm("ASC");  }
         |
          DESC
           { $$=yy_createConstStringTerm("DESC"); }
         ; 

subquery:
         '(' SELECT select_opt selection table_expr ')'
           { $$=yy_catStringTerm6_cttttc("(",$2,$3,$4,$5,")");  }
         ;

select_scalar_expr_comma_list:
                        select_scalar_expr
                          { $$=$1; }
                        |
                         select_scalar_expr_comma_list ',' select_scalar_expr
                         { $$=yy_catStringTerm3_tct($1,",",$3); }
                        ;

select_scalar_expr:
                   '@'
                    { $$ = yy_createConstStringTerm("@"); }
                   |
                    scalar_expr
                    { $$=$1; }
                   ;



scalar_expr:
	scalar_expr '+' scalar_expr
         { $$=yy_catStringTerm3_tct($1,"+",$3); }
       |
        scalar_expr '-' scalar_expr
         { $$=yy_catStringTerm3_tct($1,"-",$3); }
       |
        scalar_expr '*' scalar_expr
         { $$=yy_catStringTerm3_tct($1,"*",$3); }
       |
        scalar_expr '/' scalar_expr
         { $$=yy_catStringTerm3_tct($1,"/",$3); }
       |
        scalar_expr CONCAT_OP scalar_expr 
         { $$=yy_catStringTerm3_tct($1,"||",$3); }
       |
        '-' scalar_expr  %prec UMINUS
         { $$=yy_catStringTerm2_ct("+",$2); }
       |
        '+' scalar_expr  %prec UMINUS
         { $$=yy_catStringTerm2_ct("+",$2); }
       |
        atom
         { $$=$1; }
       |
        column_ref
         { $$=$1; }
       |
        function_ref
         { $$=$1; }
       |
        '(' scalar_expr ')'
         { $$=yy_catStringTerm3_ctc("(",$2,")"); }
       ;

table_ref_commalist:
             table_ref
              { $$=$1; }
            |
             table_ref_commalist ',' table_ref
              { $$=yy_catStringTerm3_tct($1,",",$3); }
            ;

atom_commalist:
        atom
         { $$=$1; }
       |
        atom_commalist ',' atom
         { $$=yy_catStringTerm3_tct($1,",",$3); }
       ;

atom:
	parameter_ref
        { $$=$1; }
       |
        literal
        { $$=$1; }
       |
        USER
        { $$=yy_createConstStringTerm("USER"); }
       ;

column_ref_commalist:
        column_ref
         { $$=$1; }
        |
        column_ref_commalist ',' column_ref
         { $$=yy_catStringTerm3_tct($1,",",$3); }
        ;

column_ref:
        IDENT
         { $$=$1; }
       |
        IDENT '.' IDENT
         { $$=yy_catStringTerm3_tct($1,".",$3); }
       |
        IDENT '.' IDENT '.' IDENT
         { $$=yy_catStringTerm5_tctct($1,".",$3,".",$5); }
       ; 

function_ref:
           compound_name '(' '*' ')'
         { $$=yy_catStringTerm2_tc($1,"(*)"); }
          | 
           compound_name '(' DISTINCT column_ref ')'
         { $$=yy_catStringTerm4_tctc($1,"( DISTINCT",$3,")"); }
          |
           compound_name '(' ALL scalar_expr ')'
         { $$=yy_catStringTerm4_tctc($1,"( ALL",$3,")"); }
          |
           compound_name '(' scalar_expr ')'
         { $$=yy_catStringTerm4_tctc($1,"(",$3,")"); }
          ; 

compound_name:
          IDENT
          { $$=$1; }
         |
          IDENT '.' IDENT
         { $$=yy_catStringTerm3_tct($1,".",$3); }
         ;

table_ref:
          compound_name 
         { $$=$1; }
         |
          compound_name  IDENT
         { $$=yy_catStringTerm2_tt($1,$2); }
         |
          '@'
          { $$ = yy_createConstStringTerm("@"); }
         ;

parameter_ref:
	parameter
         { $$=$1; }
        |
        parameter parameter
         { $$=yy_catStringTerm2_tt($1,$2); }
        |
        parameter INDICATOR parameter
         { $$=yy_catStringTerm3_tct($1,"INDICATOR",$3); }
        ;


parameter:
          ':' IDENT
         { $$=yy_catStringTerm2_ct(":",$2); }
         ;

literal:
        STRING
        { $$=$1; }
       |
        INTNUM
        { $$=$1; }
       |
        APPROXNUM
        { $$=$1; }
       ; 

was_error: 
          error
           { $$=NULL; }
         ;

%%

int yyerror(char *s) {
    yy_setParseError(s);
    yyval=NULL;
    return 0;
}
