%option c++ yyclass="xpl_scanner" outfile="xpl_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{
/* $Id: xpl_scanner.l,v 1.11 2017/04/14 17:56:08 ist181045 Exp $ */
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include "xpl_scanner.h"
#include "xpl_parser.tab.h"

#include <climits>

// don't change this
#define yyerror LexerError
%}

    /* Helpful rules */
DECDIG     [0-9]
HEXDIG     [0-9A-Fa-f]
HEX        "0x"{HEXDIG}+

    /* Literal regex rules */
    /* Integers (see 3.8.1 Integers) */
INTEGER    [1-9]{DECDIG}*|"0"

    /* Reals (see 3.8.2 Floating point reals) */
REAL       {DECDIG}+"."{DECDIG}*|"."{DECDIG}+
EXP        [Ee][-+]?{DECDIG}+


    /* Other regex rules */
    /* Blanks (see 3.1 Blank characters) */
BLANK      [\t\n\r ]

    /* Expression operators (see 7 Expressions) */
EXPROPS    [-+()[\]?*/%<>~&|=!@]

%x X_COMMENT X_STR X_STRMORE X_STRNO

%%

    /* Lexical conventions (3) ============================================== */
    /* Comments (3.2) ------------------------------------------------------- */
    /* Multi-line nested comments */
<INITIAL,X_STRMORE>"/*"    yy_push_state(X_COMMENT);
<X_COMMENT>"/*"            yy_push_state(X_COMMENT);
<X_COMMENT>"*/"            yy_pop_state();
<X_COMMENT>.|\n            ;


    /* Single-line comments */
<INITIAL,X_STRMORE>"//".*$ ;


    /* 3.3 Keywords --------------------------------------------------------- */
    /* Literals */
"int"                  return tTYPEINTEGER;
"real"                 return tTYPEREAL;
"string"               return tTYPESTRING;
"null"                 return tNULL;

    /* Function */
"procedure"            return tPROCEDURE;

    /* Scope */
"public"               return tPUBLIC;
"use"                  return tUSE;

    /* Conditional */
"if"                   return tIF;
"elsif"                return tELSIF;
"else"                 return tELSE;

    /* Iteration */
"while"                return tWHILE;
"sweep"                return tSWEEP;

    /* Control */
"stop"                 return tSTOP;
"next"                 return tNEXT;
"return"               return tRETURN;


    /* 3.5 Expression Operators --------------------------------------------- */
    /* Comparative and equalty */
">="                   return tGE;
"<="                   return tLE;
"=="                   return tEQ;
"!="                   return tNE;


    /* 3.6 Delimiters and terminators --------------------------------------- */
"!!"                   return tPRINTLN;


    /* 3.7 Identifiers (names) ---------------------------------------------- */
[A-Za-z_][A-Za-z0-9_]* yylval.s = new std::string(yytext); return tIDENTIFIER;


    /* 3.8 Literals --------------------------------------------------------- */
    /* 3.8.1 Integers */
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

    /* 3.8.2 Floating point reals */
({REAL}|{INTEGER}){EXP}? yylval.d = strtod(yytext, nullptr); return tREAL;

    /* 3.8.3 Strings */
\"                     yy_push_state(X_STR); yylval.s = new std::string("");
<X_STR>\"              yy_pop_state(); return tSTRING;
<X_STR>\\0             yy_push_state(X_STRNO); return tSTRING; /* NULL -> end */
<X_STR>\"{BLANK}       yy_push_state(X_STRMORE); /* Is there more? */
<X_STR>\\{HEXDIG}{1,2} *yylval.s += (char)strtol((yytext + 1), nullptr, 16); /* special hex */
<X_STR>\\\"            *yylval.s += yytext + 1; /* double quote */
<X_STR>.               *yylval.s += yytext;
<X_STR,X_STRNO>\n      yyerror("Newline in string");

<X_STRMORE>\"          yy_pop_state(); /* There's more! */
<X_STRMORE>{BLANK}     ; /* Keep looking */
<X_STRMORE>.           yy_pop_state(); yy_pop_state(); return tSTRING; /* end */

<X_STRNO>\"            yy_pop_state(); yy_pop_state();
<X_STRNO>.             ; /* Ignore whatever is found */



    /* 7 Expressions ======================================================== */
    /* Single characters (see table in 7) */
{EXPROPS}|[{};,.]      return *yytext;



    /* Whitespace (see 3.1 Blank characters) */
{BLANK}+               ; /* ignore blank characters */


    /* Aliens */
.                      yyerror("Unknown character");

%%
// Very, very dirty hack: flex is a mess generating C++ scanners.
int xpl_scanner::yywrap() { return 1; }
