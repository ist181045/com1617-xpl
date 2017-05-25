#ifndef __XPL_ADDRESSOFNODE_H__
#define __XPL_ADDRESSOFNODE_H__

#include <cdk/ast/lvalue_node.h>
#include <cdk/ast/unary_expression_node.h>

namespace xpl {

  class address_of_node: public cdk::unary_expression_node {
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
