// $Id: index_node.h,v 1.3 2017/03/24 14:54:08 ist181045 Exp $
#ifndef __XPL_INDEXNODE_H__
#define __XPL_INDEXNODE_H__

#include <cdk/ast/lvalue_node.h>
#include <cdk/ast/expression_node.h>

namespace xpl {
  class index_node : public cdk::lvalue_node {
    cdk::expression_node *_expression;
    cdk::expression_node *_offset;
    
  protected:
    inline index_node(int lineno, cdk::expression_node *expression,
        cdk::expression_node *offset)
        : cdk::lvalue_node(lineno), _expression(expression), _offset(offset) {}

  public:
    inline cdk::expression_node *expression() {
      return _expression;
    }

    inline cdk::expression_node *offset() {
      return _offset;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_index_node(this, level);
    }

  };
} // xpl

#endif