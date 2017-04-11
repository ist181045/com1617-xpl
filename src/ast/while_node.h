// $Id: while_node.h,v 1.2 2017/04/11 22:36:30 ist181045 Exp $ -*- c++ -*-
#ifndef __CDK_WHILENODE_H__
#define __CDK_WHILENODE_H__

#include <cdk/ast/expression_node.h>

namespace xpl {

  class while_node: public cdk::basic_node {
    cdk::expression_node *_condition;
    cdk::basic_node *_block;

  public:
    inline while_node(int lineno, cdk::expression_node *condition,
        cdk::basic_node *block)
        : basic_node(lineno), _condition(condition), _block(block) {
    }

  public:
    inline cdk::expression_node *condition() {
      return _condition;
    }
    inline cdk::basic_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_while_node(this, level);
    }

  };

} // xpl

#endif
