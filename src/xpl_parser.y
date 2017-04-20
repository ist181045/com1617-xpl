%{
// $Id: xpl_parser.y,v 1.26 2017/04/20 08:40:13 ist181045 Exp $
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
  basic_type  *t;  /* the type of an entity */
  
  cdk::basic_node      *node;       /* node pointer */
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;     /* lvalue nodes */
  cdk::sequence_node   *sequence;   /* sequence nodes */

  xpl::block_node   *block; /* block nodes */
};

    /* Numeric types */
%token <i>    tINTEGER
%token <d>    tREAL

    /* String */
%token <s>    tSTRING

    /* Pointers */
%token tNULL

    /* Identifier (doesn't belong.. argh!) */
%token <s> tIDENTIFIER



    /* 3 Lexical Conventions ================================================ */
    /* 3.3 Keywords (mixed with some precedence rules) ---------------------- */
    /* Literals */
%token <t> tTYPEINTEGER tTYPEREAL tTYPESTRING

    /* Function */
%token <t> tPROCEDURE

    /* Scope */
%token <i> tPUBLIC tUSE

    /* Conditional */
%token tIF
%nonassoc tIFX
%nonassoc tELSE tELSIF

    /* Iteration */
%token tWHILE tSWEEPD tSWEEPU

    /* Control */
%token tSTOP tNEXT tRETURN


    /* 3.6 Delimiters and terminators --------------------------------------- */
%token tPRINTLN



    /* 4 Syntax ============================================================= */
%type <s> str
%type <t> type

%type <block>      blck body
%type <node>       cond else iter stmt
%type <node>       arg decl func gvar var
%type <expression> expr lit
%type <lvalue>     lval
%type <sequence>   args decls exprs stmts vars



    /* 7 Expressions ======================================================== */
%right    '='               /* Assignment       */
%left     '|'               /* Logical 'or'     */
%left     '&'               /* Logical 'and'    */
%nonassoc '~'               /* Logical 'not'    */
%left     tEQ tNE           /* Equality         */
%left     tLE tGE '<' '>'   /* Comparative      */
%left     '+' '-'           /* Additive         */
%left     '*' '/' '%'       /* Multiplicative   */
%nonassoc '?' tUNARY        /* Unary precedence */
%nonassoc '['               /* Primary          */

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

    /* 4 Syntax ============================================================= */
    /* File ----------------------------------------------------------------- */
file : decls      { compiler->ast($1); }
     | /* eps */  { compiler->ast(new cdk::sequence_node(LINE)); }
     ;

decls: decl       { $$ = new cdk::sequence_node(LINE, $1);     }
     | decls decl { $$ = new cdk::sequence_node(LINE, $2, $1); }
     ;


    /* Declaration ---------------------------------------------------------- */
decl : gvar ';'   { $$ = $1; }
     | func       { $$ = $1; }
     ;


    /* Variable ------------------------------------------------------------- */
gvar : tPUBLIC type tIDENTIFIER          { $$ = new xpl::vardecl_node(LINE, $1, $2, $3); }
     | tPUBLIC type tIDENTIFIER '=' expr { $$ = new xpl::var_node(LINE, $1, $2, $3, $5); }
     | tUSE type tIDENTIFIER             { $$ = new xpl::vardecl_node(LINE, $1, $2, $3); }
     | var                               { $$ = $1; }
     ;

var  : type tIDENTIFIER          { $$ = new xpl::vardecl_node(LINE, 0, $1, $2); }
     | type tIDENTIFIER '=' expr { $$ = new xpl::var_node(LINE, 0, $1, $2, $4); }
     ;


    /* Function ------------------------------------------------------------- */
func : tPUBLIC tPROCEDURE tIDENTIFIER '(' args ')'        { $$ = new xpl::fundecl_node (LINE, $1, $2, $3, $5);         }
     | tPUBLIC tPROCEDURE tIDENTIFIER '(' args ')' body   { $$ = new xpl::function_node(LINE, $1, $2, $3, $5, $7);     }
     | tPUBLIC type tIDENTIFIER '(' args ')'              { $$ = new xpl::fundecl_node (LINE, $1, $2, $3, $5);         }
     | tPUBLIC type tIDENTIFIER '(' args ')' body         { $$ = new xpl::function_node(LINE, $1, $2, $3, $5, $7);     }
     | tPUBLIC type tIDENTIFIER '(' args ')' '=' lit body { $$ = new xpl::function_node(LINE, $1, $2, $3, $5, $9, $8); }
     | tUSE tPROCEDURE tIDENTIFIER '(' args ')'           { $$ = new xpl::fundecl_node (LINE, $1, $2, $3, $5);         }
     | tUSE type tIDENTIFIER '(' args ')'                 { $$ = new xpl::fundecl_node (LINE, $1, $2, $3, $5);         }
     | tPROCEDURE tIDENTIFIER '(' args ')'                { $$ = new xpl::fundecl_node (LINE,  0, $1, $2, $4);         }
     | tPROCEDURE tIDENTIFIER '(' args ')' body           { $$ = new xpl::function_node(LINE,  0, $1, $2, $4, $6);     }
     | type tIDENTIFIER '(' args ')'                      { $$ = new xpl::fundecl_node (LINE,  0, $1, $2, $4);         }
     | type tIDENTIFIER '(' args ')' body                 { $$ = new xpl::function_node(LINE,  0, $1, $2, $4, $6);     }
     | type tIDENTIFIER '(' args ')' '=' lit body         { $$ = new xpl::function_node(LINE,  0, $1, $2, $4, $8, $7); }
     ;


type : tTYPEINTEGER { $$ = $1; }
     | tTYPEREAL    { $$ = $1; }
     | tTYPESTRING  { $$ = $1; }
     | '[' type ']' { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2; }
     ;


args : arg          { $$ = new cdk::sequence_node(LINE, $1);     }
     | args ',' arg { $$ = new cdk::sequence_node(LINE, $3, $1); }
     | /* eps */    { $$ = new cdk::sequence_node(LINE);         }
     ;

arg  : type tIDENTIFIER { $$ = new xpl::vardecl_node(LINE, 0, $1, $2); }
     ;


lit  : str          { $$ = new cdk::string_node(LINE, $1);  }
     | tINTEGER     { $$ = new cdk::integer_node(LINE, $1); }
     | tREAL        { $$ = new cdk::double_node(LINE, $1);  }
     | tNULL        { $$ = new xpl::null_node(LINE);        }
     ;

str  : tSTRING     { $$ = $1; }
     | str tSTRING { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
     ;


    /* Body ----------------------------------------------------------------- */
body : blck      { $$ = $1; }
     ;


    /* Block ---------------------------------------------------------------- */
blck : '{' vars stmts '}'   { $$ = new xpl::block_node(LINE, $2, $3); }
     | '{' vars  '}'        { $$ = new xpl::block_node(LINE, $2, new cdk::sequence_node(LINE)); }
     | '{' stmts '}'        { $$ = new xpl::block_node(LINE, new cdk::sequence_node(LINE), $2); }
     | '{' '}'              { $$ = new xpl::block_node(LINE, new cdk::sequence_node(LINE), new cdk::sequence_node(LINE)); }
     ;

vars : var ';'              { $$ = new cdk::sequence_node(LINE, $1);     }
     | vars var ';'         { $$ = new cdk::sequence_node(LINE, $2, $1); }
     ;

stmts: stmt                 { $$ = new cdk::sequence_node(LINE, $1);     }
     | stmts stmt           { $$ = new cdk::sequence_node(LINE, $2, $1); }
     ;


    /* Statements ----------------------------------------------------------- */
stmt : expr ';'      { $$ = new xpl::evaluation_node(LINE, $1);  }
     | expr '!'      { $$ = new xpl::print_node(LINE, $1);       }
     | expr tPRINTLN { $$ = new xpl::print_node(LINE, $1, true); }
     | tNEXT         { $$ = new xpl::next_node(LINE);   }
     | tSTOP         { $$ = new xpl::stop_node(LINE);   }
     | tRETURN       { $$ = new xpl::return_node(LINE); }
     | cond          { $$ = $1; }
     | iter          { $$ = $1; }
     | blck          { $$ = $1; }
     ;


    /* Conditional statements */
cond : tIF '(' expr ')' stmt %prec tIFX    { $$ = new xpl::if_node(LINE, $3, $5);          }
     | tIF '(' expr ')' stmt else          { $$ = new xpl::if_else_node(LINE, $3, $5, $6); }
     ;

else : tELSE stmt                          { $$ = $2; }
     | tELSIF '(' expr ')' stmt %prec tIFX { $$ = new xpl::if_node(LINE, $3, $5);          }
     | tELSIF '(' expr ')' stmt else       { $$ = new xpl::if_else_node(LINE, $3, $5, $6); }
     ;


    /* Iteration statements */
iter : tWHILE '(' expr ')' stmt                             { $$ = new xpl::while_node(LINE, $3, $5); }
     | tSWEEPU '(' lval ':' expr ':' expr ')' stmt          { $$ = new xpl::sweep_up_node(LINE, $3, $5, $7, $9);        }
     | tSWEEPU '(' lval ':' expr ':' expr ':' expr ')' stmt { $$ = new xpl::sweep_up_node(LINE, $3, $5, $7, $11, $9);   }
     | tSWEEPD '(' lval ':' expr ':' expr ')' stmt          { $$ = new xpl::sweep_down_node(LINE, $3, $5, $7, $9);      }
     | tSWEEPD '(' lval ':' expr ':' expr ':' expr ')' stmt { $$ = new xpl::sweep_down_node(LINE, $3, $5, $7, $11, $9); }
     ;


    /* Expressions ---------------------------------------------------------- */
expr : lit                       { $$ = $1; }
     | lval                      { $$ = $1; }
     | '(' expr ')'              { $$ = $2; }
     | '[' expr ']'              { $$ = new xpl::malloc_node(LINE, $2);         }
     | '@'                       { $$ = new xpl::read_node(LINE);               }
     | '+' expr %prec tUNARY     { $$ = new xpl::identity_node(LINE, $2);       }
     | '-' expr %prec tUNARY     { $$ = new cdk::neg_node(LINE, $2);            }
     | '~' expr                  { $$ = new cdk::not_node(LINE, $2);            }
     | expr '|' expr             { $$ = new cdk::or_node(LINE, $1, $3);         }
     | expr '&' expr             { $$ = new cdk::and_node(LINE, $1, $3);        }
     | expr '+' expr             { $$ = new cdk::add_node(LINE, $1, $3);        }
     | expr '-' expr             { $$ = new cdk::sub_node(LINE, $1, $3);        }
     | expr '*' expr             { $$ = new cdk::mul_node(LINE, $1, $3);        }
     | expr '/' expr             { $$ = new cdk::div_node(LINE, $1, $3);        }
     | expr '%' expr             { $$ = new cdk::mod_node(LINE, $1, $3);        }
     | expr '<' expr             { $$ = new cdk::lt_node(LINE, $1, $3);         }
     | expr '>' expr             { $$ = new cdk::gt_node(LINE, $1, $3);         }
     | expr tGE expr             { $$ = new cdk::ge_node(LINE, $1, $3);         }
     | expr tLE expr             { $$ = new cdk::le_node(LINE, $1, $3);         }
     | expr tNE expr             { $$ = new cdk::ne_node(LINE, $1, $3);         }
     | expr tEQ expr             { $$ = new cdk::eq_node(LINE, $1, $3);         }
     | tIDENTIFIER '(' exprs ')' { $$ = new xpl::funcall_node(LINE, $1, $3);    }
     | lval '?'                  { $$ = new xpl::address_of_node(LINE, $1);     }
     | lval '=' expr             { $$ = new cdk::assignment_node(LINE, $1, $3); }
     ;

exprs: expr                      { $$ = new cdk::sequence_node(LINE, $1);       }
     | exprs ',' expr            { $$ = new cdk::sequence_node(LINE, $3, $1);   }
     | /* eps */                 { $$ = new cdk::sequence_node(LINE);           }
     ;

lval : tIDENTIFIER               { $$ = new cdk::identifier_node(LINE, $1);     }
     | expr '[' expr ']'         { $$ = new xpl::index_node(LINE, $1, $3);      }
     ;

%%
