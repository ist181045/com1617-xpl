// $Id: position_node.h,v 1.2 2017/03/27 01:58:02 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_POSITIONNODE_H__
#define __XPL_POSITIONNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace xpl {

  class position_node : public cdk::unary_expression_node {
  public:
    inline position_node(int lineno, cdk::lvalue_node *lvalue)
        : cdk::unary_expression_node(lineno, lvalue) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_position_node(this, level);
    }

  };

} // xpl

#endif
