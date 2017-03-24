// $Id: malloc_node.h,v 1.2 2017/03/24 14:54:08 ist181045 Exp $ -*- c++ -*-

#ifndef __XPL_MALLOCNODE_H__
#define __XPL_MALLOCNODE_H__ 

#include <cdk/ast/expression_node.h>

namespace xpl {

  class malloc_node : public cdk::expression_node {
    cdk::expression_node *_expression;

  protected:
    inline malloc_node(int lineno, cdk::expression_node *expression)
      : cdk::expression_node(lineno), _expression(expression) {}

  public:
    inline cdk::expression_node *expression() {
      return _expression;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_malloc_node(this, level);
    }

  };

} // xpl

#endif
