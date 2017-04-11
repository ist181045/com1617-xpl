// $Id: function_node.h,v 1.7 2017/04/11 16:17:01 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_FUNCTIONNODE_H__
#define __XPL_FUNCTIONNODE_H__

#include <string>

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/identifier_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>

namespace xpl {

  class function_node: public cdk::basic_node {
    int _scope;
    basic_type *_type;
    std::string *_name;
    cdk::sequence_node *_arguments;
    cdk::expression_node *_retval;
    block_node *_body;

  public:
    inline function_node(int lineno, int scope, basic_type *type,
        std::string *name, cdk::sequence_node *arguments,
        cdk::expression_node *retval, block_node *body) :
        cdk::basic_node(lineno), _scope(scope), _type(type), _name(name),
        _arguments(arguments), _retval(retval), _body(body) {}

  public:
    inline const int scope() const {
      return _scope;
    }

    inline basic_type *type() {
      return _type;
    }

    inline cdk::identifier_node *name() {
      return _name;
    }

    inline cdk::sequence_node *arguments() {
      return _arguments;
    }
    
    inline cdk::expression_node *retval() {
      return _retval;
    }

    inline block_node *body() {
      return _body;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_node(this, level);
    }

  };

} // xpl

#endif