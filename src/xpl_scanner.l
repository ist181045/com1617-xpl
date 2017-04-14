%option c++ yyclass="xpl_scanner" outfile="xpl_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{
/* $Id: xpl_scanner.l,v 1.7 2017/04/14 14:31:54 ist181045 Exp $ */
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include "xpl_scanner.h"
#include "xpl_parser.tab.h"

#include <climits>
#include <cmath>

// don't change this
#define yyerror LexerError
%}

    /* Literal regex */
INTEGER    [1-9][0-9]*|"0"
HEXC       [0-9A-Fa-f]
HEX        "0x"{HEXC}+

REAL       [0-9]+"."[0-9]*|"."[0-9]+
EXP        [Ee][-+]?[0-9]+

    /* Other regex */
SINGLES    [-()[\]<>=+*/%;!@?|~&{}.]

%x X_COMMENT X_STR X_STRMORE X_STRNO

%%
    { set_debug(1); }

    /* Comments */
    /* Multi-line nested comments */
<INITIAL,X_STRMORE>"/*"    yy_push_state(X_COMMENT);
<X_COMMENT>"/*"            yy_push_state(X_COMMENT);
<X_COMMENT>"*/"            yy_pop_state();
<X_COMMENT>.|\n            ;


    /* Single-line comments */
<INITIAL,X_STRMORE>"//".*$ ;



    /* Operators */
    /* Relational operators */
">="                   return tGE;
"<="                   return tLE;
"=="                   return tEQ;
"!="                   return tNE;

    /* I/O operators */
"!!"                   return tPRINTLN;



    /* Keywords */
    /* Literal identifier keywords */
"int"                  return tTYPEINTEGER;
"real"                 return tTYPEREAL;
"string"               return tTYPESTRING;
    
    /* Literals */
"null"                 return tNULL;


    /* Scope keywords */
"public"               return tPUBLIC;
"use"                  return tUSE;


    /* Conditional statement keywords */
"if"                   return tIF;
"else"                 return tELSE;
"elsif"                return tELSIF;


    /* Control statement keywords */
"stop"                 return tSTOP;
"next"                 return tNEXT;
"return"               return tRETURN;


    /* Iteration statement keywords */
"while"                return tWHILE;
"sweep"                return tSWEEP;


    /* Others */
"procedure"            return tPROCEDURE;


    /* Identifiers */
[A-Za-z_][A-Za-z0-9_]* yylval.s = new std::string(yytext); return tIDENTIFIER;



    /* Literals */
    /* Strings */
\"                     yy_push_state(X_STR); yylval.s = new std::string("");
<X_STR>\"              yy_pop_state(); return tSTRING;
<X_STR>\\0             yy_push_state(X_STRNO); return tSTRING; /* NULL -> done here */
<X_STR>\"[[:space:]]   yy_push_state(X_STRMORE); /* Is there more? */
<X_STR>\\{HEXC}{1,2}   *yylval.s += (char)strtol((yytext + 1), nullptr, 16); /* special hex */
<X_STR>\\\"            *yylval.s += yytext + 1; /* double quote */
<X_STR>.               *yylval.s += yytext;
<X_STR,X_STRNO>\n      yyerror("Newline in string");

<X_STRMORE>\"          yy_pop_state(); /* There's more! */
<X_STRMORE>[[:space:]] ; /* Keep looking */
<X_STRMORE>.           yy_pop_state(); yy_pop_state(); return tSTRING; /* end */

<X_STRNO>\"            yy_pop_state(); yy_pop_state();
<X_STRNO>.             ; /* Ignore whatever is found */


    /* Integers */
"0"+{INTEGER}          yyerror("Integers mustn\'t have a single leading 0");
{INTEGER}|{HEX}        {
    short b = yytext[1] && yytext[1] == 'x' ? 16 : 10;
    long l = strtol(yytext, nullptr, b);

    if (l > INT_MAX) yyerror("Integer overflow");
    else if (l < INT_MIN) yyerror("Integer underflow");
    else {
      yylval.i = (int)l;
      return tINTEGER;
    }
}


    /* Reals (safe, flex will match rules above first) */
({REAL}|{INTEGER}){EXP}? yylval.d = strtod(yytext, nullptr); return tREAL;


    /* Others */
    /* Single characters */
{SINGLES}              return *yytext;


    /* Whitespace */
[[:space:]]+           ; /* ignore horizontal and vertical whitespace */


    /* Aliens */
.                      yyerror("Unknown character");

%%
// Very, very dirty hack: flex is a mess generating C++ scanners.
int xpl_scanner::yywrap() { return 1; }
