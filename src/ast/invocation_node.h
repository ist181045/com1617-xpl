// $Id: invocation_node.h,v 1.1 2017/03/24 14:42:34 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_INVOCNODE_H__
#define __XPL_INVOCNODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/identifier_node.h>
#include <cdk/ast/sequence_node.h>

namespace xpl {
  class invocation_node : public cdk::expression_node {
    cdk::identifier_node *_function;
    cdk::sequence_node *_arguments;

  protected:
    inline invocation_node(int lineno, cdk::identifier_node *function,
        cdk::sequence_node *arguments)
        : cdk::expression_node(lineno), _function(function),
        _arguments(arguments) {}

  public:
    inline cdk::invocation_node *function() {
      return _function;
    }

    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_invocation_node(this, level);
    }

  };
} // xpl

#endif