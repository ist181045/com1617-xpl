%{
// $Id: xpl_parser.y,v 1.11 2017/04/15 12:04:39 ist181045 Exp $
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

  xpl::block_node *block; /* block nodes */
  xpl::vardecl_node *var; /* variable nodes */
  xpl::null_node *null;   /* null nodes */
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
%token <t> tPROCEDURE

    /* Scope */
%nonassoc <i> tPUBLIC tUSE

    /* Conditional */
%token tIF
%nonassoc tIFX
%nonassoc tELSE
%nonassoc tELSIF

    /* Iteration */
%token tWHILE tSWEEPD tSWEEPU

    /* Control */
%token tSTOP tNEXT tRETURN


    /* 3.6 Delimiters and terminators --------------------------------------- */
%token tPRINTLN



    /* 4 Syntax ============================================================= */
%type <block>      body blck
%type <node>       cond decl elsif func iter stmt
%type <expression> expr lit
%type <i>          qual
%type <t>          type
%type <lvalue>     lval
%type <sequence>   decls elifs exprs stmts vars
%type <var>        var



    /* 7 Expressions ======================================================== */
%right    '='                      /* Assignment             */
%left     '|'                      /* Logical 'or'           */
%left     '&'                      /* Logical 'and'          */
%nonassoc '~'                      /* Logical 'not'          */
%left     tEQ tNE tLE tGE '<' '>'  /* Comparative + Equality */
%left     '+' '-'                  /* Additive               */
%left     '*' '/' '%'              /* Multiplicative         */
%nonassoc tUNARY                   /* Unary precedence token */
%nonassoc '{' '}'                  /* Block precedence, solves conflicts in func */
%nonassoc '(' ')' '[' ']'          /* Primary */

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

    /* 4 Syntax ============================================================= */
    /* File ----------------------------------------------------------------- */
file : decls      { compiler->ast($1); }
     | /* eps */  { compiler->ast(new cdk::sequence_node(LINE)); }
     ;

decls: decl       { $$ = new cdk::sequence_node(LINE, $1); }
     | decls decl { $$ = new cdk::sequence_node(LINE, $2, $1); }
     ;


    /* Declaration ---------------------------------------------------------- */
decl : var ';'   { $$ = $1; }
     | func      { $$ = $1; }
     ;


    /* Variable ------------------------------------------------------------- */
var  : type tIDENTIFIER               { $$ = new xpl::vardecl_node(LINE, 0, $1, $2);      }
     | type tIDENTIFIER '=' expr      { $$ = new xpl::vardecl_node(LINE, 0, $1, $2, $4);  }
     | qual type tIDENTIFIER          { $$ = new xpl::vardecl_node(LINE, $1, $2, $3);     }
     | qual type tIDENTIFIER '=' expr { $$ = new xpl::vardecl_node(LINE, $1, $2, $3, $5); }
     ;


    /* Function ------------------------------------------------------------- */
func : tPROCEDURE tIDENTIFIER '(' vars ')'             { $$ = new xpl::fundecl_node (LINE, 0, $1, $2, $4);         }
     | tPROCEDURE tIDENTIFIER '(' vars ')' body        { $$ = new xpl::function_node(LINE, 0, $1, $2, $4, $6);     }
     | type tIDENTIFIER '(' vars ')'                   { $$ = new xpl::fundecl_node (LINE, 0, $1, $2, $4);         }
     | type tIDENTIFIER '(' vars ')' body              { $$ = new xpl::function_node(LINE, 0, $1, $2, $4, $6);     }
     | type tIDENTIFIER '(' vars ')' '=' lit           { $$ = new xpl::fundecl_node (LINE, 0, $1, $2, $4, $7);     }
     | type tIDENTIFIER '(' vars ')' '=' lit body      { $$ = new xpl::function_node(LINE, 0, $1, $2, $4, $8, $7); }
     | qual tPROCEDURE tIDENTIFIER '(' vars ')'        { $$ = new xpl::fundecl_node (LINE, $1, $2, $3, $5);        }
     | qual tPROCEDURE tIDENTIFIER '(' vars ')' body   { $$ = new xpl::function_node(LINE, $1, $2, $3, $5, $7);    }
     | qual type tIDENTIFIER '(' vars ')'              { $$ = new xpl::fundecl_node (LINE, $1, $2, $3, $5);        }
     | qual type tIDENTIFIER '(' vars ')' body         { $$ = new xpl::function_node(LINE, $1, $2, $3, $5, $7);    }
     | qual type tIDENTIFIER '(' vars ')' '=' lit      { $$ = new xpl::fundecl_node (LINE, $1, $2, $3, $5, $8);    }
     ;

qual : tPUBLIC      { $$ = 1; }
     | tUSE         { $$ = 2; }
     ;

type : tTYPEINTEGER { $$ = new basic_type(4, basic_type::TYPE_INT);    }
     | tTYPEREAL    { $$ = new basic_type(8, basic_type::TYPE_DOUBLE); }
     | tTYPESTRING  { $$ = new basic_type(0, basic_type::TYPE_STRING);  }
     | '[' type ']' {
       $$ = new basic_type(4, basic_type::TYPE_POINTER);
       $$->_subtype = $2;
     }
     ;

vars : var          { $$ = new cdk::sequence_node(LINE, $1);     }
     | vars ',' var { $$ = new cdk::sequence_node(LINE, $3, $1); }
     | /* eps */    { $$ = new cdk::sequence_node(LINE);         }
     ;

lit  : tINTEGER     { $$ = new cdk::integer_node(LINE, $1); }
     | tREAL        { $$ = new cdk::double_node(LINE, $1);  }
     | tSTRING      { $$ = new cdk::string_node(LINE, $1);  }
     | tNULL        { $$ = new xpl::null_node(LINE);        }
     ;


    /* Body ----------------------------------------------------------------- */
body : blck      { $$ = $1; }
     ;


    /* Block ---------------------------------------------------------------- */
blck : '{' decls stmts '}' { $$ = new xpl::block_node(LINE, $2, $3); }
     | '{' decls '}'       { $$ = new xpl::block_node(LINE, $2, new cdk::sequence_node(LINE)); }
     | '{' stmts '}'       { $$ = new xpl::block_node(LINE, new cdk::sequence_node(LINE), $2); }
     | '{' '}'             { $$ = new xpl::block_node(LINE, new cdk::sequence_node(LINE), new cdk::sequence_node(LINE)); }
     ;

stmts: stmt                { $$ = new cdk::sequence_node(LINE, $1);     }
     | stmts stmt          { $$ = new cdk::sequence_node(LINE, $2, $1); }
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
cond : tIF '(' expr ')' stmt %prec tIFX              { $$ = new xpl::if_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt tELSE stmt              { $$ = new xpl::if_else_node(LINE, $3, $5, $7); }
     | tIF '(' expr ')' stmt elifs %prec tIFX        { $$ = new xpl::if_else_node(LINE, $3, $5, $6); }
     | tIF '(' expr ')' stmt elifs tELSE stmt        { $$ = new xpl::if_else_node(LINE, $3, $5, new cdk::sequence_node(LINE, $8, $6)); }
     ;

elifs: elsif                     { $$ = new cdk::sequence_node(LINE, $1);     }
     | elifs elsif               { $$ = new cdk::sequence_node(LINE, $2, $1); }
     ;

elsif: tELSIF '(' expr ')' stmt  { $$ = new xpl::if_node(LINE, $3, $5); }
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
     | lval '[' expr ']'         { $$ = new xpl::index_node(LINE, $1, $3);      }
     | lval '=' expr             { $$ = new cdk::assignment_node(LINE, $1, $3); }
     ;

exprs: expr                      { $$ = new cdk::sequence_node(LINE, $1);       }
     | exprs ',' expr            { $$ = new cdk::sequence_node(LINE, $3, $1);   }
     ;

lval : tIDENTIFIER               { $$ = new cdk::identifier_node(LINE, $1);     }
     ;

%%
