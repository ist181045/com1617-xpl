// $Id: invocation_node.h,v 1.4 2017/04/11 16:17:01 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_INVOCNODE_H__
#define __XPL_INVOCNODE_H__

#include <string>

#include <cdk/ast/expression_node.h>
#include <cdk/ast/identifier_node.h>
#include <cdk/ast/sequence_node.h>

namespace xpl {

  class invocation_node : public cdk::expression_node {
    std::string *_function;
    cdk::sequence_node *_arguments;

  public:
    inline invocation_node(int lineno, std::string *function,
        cdk::sequence_node *arguments) : cdk::expression_node(lineno),
        _function(function), _arguments(arguments) {}

  public:
    inline std::string *function() {
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