// $Id: address_of_node.h,v 1.3 2017/03/27 18:14:50 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_ADDRESSOFNODE_H__
#define __XPL_ADDRESSOFNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace xpl {

  class address_of_node : public cdk::unary_expression_node {
  public:
    inline address_of_node(int lineno, cdk::lvalue_node *lvalue)
        : cdk::unary_expression_node(lineno, lvalue) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_address_of_node(this, level);
    }

  };

} // xpl

#endif
