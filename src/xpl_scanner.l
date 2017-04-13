%option c++ yyclass="xpl_scanner" outfile="xpl_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{
/* $Id: xpl_scanner.l,v 1.2 2017/04/13 16:40:45 ist181045 Exp $ */
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include "xpl_scanner.h"
#include "xpl_parser.tab.h"

// don't change this
#define yyerror LexerError
%}
%x X_STRING X_COMMENT
%%

  /* Comments */
  /* Multi-line nested comments */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|\n        ;


  /* Single-line comments */

"//".*$                ;



  /* Operators */
  /* Relational operators */

">="                   return tGE;
"<="                   return tLE;
"=="                   return tEQ;
"!="                   return tNE;

  /* I/O operators */

"!!"                   return tPRINTLN;



  /* Keywords */
  /* Scope keywords */

"public"               return tPUBLIC;
"use"                  return tUSE;


  /* Iteration statement keywords */

"while"                return tWHILE;
"sweep"                return tSWEEP;


  /* Conditional statement keywords */

"if"                   return tIF;
"else"                 return tELSE;
"elsif"                return tELSIF;


  /* Control statement keywords */

"next"                 return tNEXT;
"stop"                 return tSTOP;
"return"               return tRETURN;


  /* Identifiers */

[A-Za-z_][A-Za-z0-9_]* yylval.s = new std::string(yytext); return tIDENTIFIER;



  /* Literals */
  /* Strings */

\"                     yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\"           yy_pop_state(); return tSTRING;
<X_STRING>\\\"         *yylval.s += yytext + 1;
<X_STRING>.            *yylval.s += yytext;
<X_STRING>\n           yyerror("Newline in string");


[-()<>=+*/%;{}.]       return *yytext;

[ \t\n]+               ; /* ignore whitespace */


  /* Aliens */

.                      yyerror("Unknown character");

%%
// Very, very dirty hack: flex is a mess generating C++ scanners.
int xpl_scanner::yywrap() { return 1; }
