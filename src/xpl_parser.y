%{
// $Id: xpl_parser.y,v 1.9 2017/04/14 17:12:19 ist181045 Exp $
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE       compiler->scanner()->lineno()
#define yylex()    compiler->scanner()->scan()
#define yyerror(s) compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

    /* 1 Data Types ========================================================= */
%union {
  int          i;  /* integer value */
  double       d;  /* double/real value */
  std::string *s;  /* symbol name or string literal */
  
  cdk::basic_node      *node;       /* node pointer */
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;     /* lvalue nodes */
  cdk::sequence_node   *sequence;   /* sequence nodes */

  xpl::null_node *null; /* null nodes */
};

    /* Numeric types */
%token <i>    tINTEGER
%token <d>    tREAL

    /* String */
%token <s>    tSTRING

    /* Pointers */
%token <null> tNULL

    /* Identifier (doesn't belong.. argh!) */
%token <s>    tIDENTIFIER



    /* 3 Lexical Conventions ================================================ */
    /* 3.3 Keywords --------------------------------------------------------- */
    /* Literals */
%token tTYPEINTEGER tTYPEREAL tTYPESTRING

    /* Function */
%token tPROCEDURE

    /* Scope */
%token tPUBLIC tUSE

    /* Conditional */
%token tIF
%nonassoc tIFX
%nonassoc tELSE
%nonassoc tELSIF

    /* Iteration */
%token tWHILE tSWEEP

    /* Control */
%token tSTOP tNEXT tRETURN

    /* 3.6 Delimiters and terminators --------------------------------------- */
%token tPRINTLN



    /* 4 Syntax ============================================================= */
%type <node> stmt
%type <sequence> list
%type <expression> expr
%type <lvalue> lval



    /* 7 Expressions ======================================================== */
%right '='
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

    /* 4 Syntax ============================================================= */

file : list      { compiler->ast($1); }

list : stmt      { $$ = new cdk::sequence_node(LINE, $1);     }
     | list stmt { $$ = new cdk::sequence_node(LINE, $2, $1); }
     ;

stmt : expr ';'                         { $$ = new xpl::evaluation_node(LINE, $1);      }
     | tWHILE '(' expr ')' stmt         { $$ = new xpl::while_node(LINE, $3, $5);       }
     | tIF '(' expr ')' stmt %prec tIFX { $$ = new xpl::if_node(LINE, $3, $5);          }
     | tIF '(' expr ')' stmt tELSE stmt { $$ = new xpl::if_else_node(LINE, $3, $5, $7); }
     | '{' list '}'                     { $$ = $2; }
     ;

expr : tINTEGER                { $$ = new cdk::integer_node(LINE, $1); }
     | tSTRING                 { $$ = new cdk::string_node(LINE, $1);  }
     | tREAL                   { $$ = new cdk::double_node(LINE, $1);  }
     | '-' expr %prec tUNARY   { $$ = new cdk::neg_node(LINE, $2);     }
     | expr '+' expr           { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr           { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr           { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr           { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr           { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr           { $$ = new cdk::lt_node(LINE, $1, $3);  }
     | expr '>' expr           { $$ = new cdk::gt_node(LINE, $1, $3);  }
     | expr tGE expr           { $$ = new cdk::ge_node(LINE, $1, $3);  }
     | expr tLE expr           { $$ = new cdk::le_node(LINE, $1, $3);  }
     | expr tNE expr           { $$ = new cdk::ne_node(LINE, $1, $3);  }
     | expr tEQ expr           { $$ = new cdk::eq_node(LINE, $1, $3);  }
     | '(' expr ')'            { $$ = $2; }
     | lval                    { $$ = new cdk::rvalue_node(LINE, $1);  }  //FIXME
     | lval '=' expr           { $$ = new cdk::assignment_node(LINE, $1, $3); }
     ;

lval : tIDENTIFIER             { $$ = new cdk::identifier_node(LINE, $1); }
     ;

%%
