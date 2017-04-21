%option c++ yyclass="xpl_scanner" outfile="xpl_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{
/* $Id: xpl_scanner.l,v 1.29 2017/04/21 12:55:37 ist181045 Exp $ */
// make relevant includes before including the parser's tab file
#include <limits>
#include <string>
#include <sstream>

#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include "xpl_scanner.h"
#include "xpl_parser.tab.h"

// don't change this
#define yyerror LexerError
%}

    /* Helpful rules */
DECDIG     [0-9]
HEXDIG     {DECDIG}|[A-Fa-f]

    /* Literal regex rules */
    /* Integers (see 3.8.1 Integers) */
INT        {DECDIG}+
HEX        0x{HEXDIG}+

    /* Reals (see 3.8.2 Floating point reals) */
REAL       {DECDIG}+"."{DECDIG}*|"."{DECDIG}+
EXP        [Ee][-+]?{DECDIG}+


    /* Other regex rules */
    /* Blanks (see 3.1 Blank characters) */
BLANK      [\t\n\r ]

    /* Expression operators (see 7 Expressions) */
EXPROPS    [-+()[\]?*/%<>~&|=!@]

%x X_COMMENT X_STR X_STRNO

%%

    /* Lexical conventions (3) ============================================== */
    /* Comments (3.2) ------------------------------------------------------- */
    /* Multi-line nested comments */
"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|\n        ;


    /* Single-line comments */
"//".*$ ;


    /* 3.3 Keywords --------------------------------------------------------- */
    /* Literals */
"int"                  yylval.t = new basic_type(4, basic_type::TYPE_INT);    return tTYPEINTEGER;
"real"                 yylval.t = new basic_type(8, basic_type::TYPE_DOUBLE); return tTYPEREAL;
"string"               yylval.t = new basic_type(4, basic_type::TYPE_STRING); return tTYPESTRING;
"null"                 return tNULL; /* Keyword representing null pointer (0) */

    /* Function */
"procedure"            yylval.t = new basic_type(0, basic_type::TYPE_VOID); return tPROCEDURE;

    /* Scope */
"public"               yylval.i = 1; return tPUBLIC;
"use"                  yylval.i = 2; return tUSE;

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
0+{INT}                yyerror("Integers mustn\'t start with 0");
0|{INT}|{HEX}          {
    try {
        yylval.i = std::stoi(yytext, nullptr, 0);
        return tINTEGER;
    } catch (const std::out_of_range &oor) {
        std::ostringstream msg;
        msg << yylineno << ": Integer overflow";
        yyerror(msg.str().c_str());
    }
}

    /* 3.8.2 Floating point reals */
({REAL}|{INT}){EXP}?   {
    try {
        yylval.d = std::stod(yytext, nullptr);
        return tREAL;
    } catch (const std::out_of_range &oor) {
        std::ostringstream msg;
        msg << yylineno << ": Real too large";
        yyerror(msg.str().c_str());
    }
}

    /* 3.8.3 Strings */
\"                     yy_push_state(X_STR); yylval.s = new std::string("");
<X_STR>\"              yy_pop_state(); return tSTRING;
<X_STR>\\t             *yylval.s += '\t'; /* Explicit interpretation.. ugly */
<X_STR>\\n             *yylval.s += '\n';
<X_STR>\\r             *yylval.s += '\r';
<X_STR>\\(\"|\\)       *yylval.s += yytext + 1; /* double quote & backslash */
<X_STR>\\{HEXDIG}{1,2} {
    int hex = std::stoi(yytext + 1, nullptr, 16);
    if (hex == 0) yy_push_state(X_STRNO);
    else *yylval.s += static_cast<char>(hex);
}
<X_STR>.               *yylval.s += yytext;
<X_STR,X_STRNO>\n      yyerror("Newline in string");

<X_STRNO>\"            yy_pop_state(); yy_pop_state(); return tSTRING;
<X_STRNO>.             ; /* Ignore whatever is found */



    /* 7 Expressions ======================================================== */
    /* Single characters (see table in 7) */
{EXPROPS}|[{}:;,.]     return *yytext;



    /* Whitespace (see 3.1 Blank characters) */
{BLANK}+               ; /* ignore blank characters */


    /* Aliens */
.                      yyerror("Unknown character");

%%
// Very, very dirty hack: flex is a mess generating C++ scanners.
int xpl_scanner::yywrap() { return 1; }
