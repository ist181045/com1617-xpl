// $Id: if_node.h,v 1.2 2017/04/11 22:36:30 ist181045 Exp $ -*- c++ -*-
#ifndef __CDK_IFNODE_H__
#define __CDK_IFNODE_H__

#include <cdk/ast/expression_node.h>

namespace xpl {

  class if_node: public cdk::basic_node {
    cdk::expression_node *_condition;
    cdk::basic_node *_block;

  public:
    inline if_node(int lineno, cdk::expression_node *condition,
        cdk::basic_node *block)
        : cdk::basic_node(lineno), _condition(condition), _block(block) {
    }

  public:
    inline cdk::expression_node *condition() {
      return _condition;
    }
    inline cdk::basic_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_if_node(this, level);
    }

  };

} // xpl

#endif
