// $Id: index_node.h,v 1.5 2017/04/11 22:36:30 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_INDEXNODE_H__
#define __XPL_INDEXNODE_H__

#include <cdk/ast/lvalue_node.h>
#include <cdk/ast/expression_node.h>

namespace xpl {

  class index_node: public cdk::lvalue_node {
    cdk::expression_node *_expression;
    cdk::expression_node *_offset;
    
  public:
    inline index_node(int lineno, cdk::expression_node *expression,
        cdk::expression_node *offset)
        : cdk::lvalue_node(lineno), _expression(expression), _offset(offset) {
    }

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