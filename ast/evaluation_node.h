// $Id: evaluation_node.h,v 1.4 2017/04/11 22:45:15 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_EVALUATIONNODE_H__
#define __XPL_EVALUATIONNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>

namespace xpl {

  class evaluation_node: public cdk::basic_node {
    cdk::expression_node *_argument;

  public:
    inline evaluation_node(int lineno, cdk::expression_node *argument)
        : cdk::basic_node(lineno), _argument(argument) {
    }

  public:
    inline cdk::expression_node *argument() {
      return _argument;
    }
    
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_evaluation_node(this, level);
    }

  };

} // xpl

#endif
