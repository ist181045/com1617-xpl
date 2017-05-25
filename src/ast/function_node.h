#ifndef __XPL_FUNCTIONNODE_H__
#define __XPL_FUNCTIONNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/basic_type.h>

#include <string>

#include "block_node.h"
#include "null_node.h"

namespace xpl {

  class function_node: public cdk::basic_node {
    int _scope;
    basic_type *_type;
    std::string _name;
    cdk::sequence_node *_arguments;
    block_node *_body;
    cdk::expression_node *_retval;

    inline void check_retval() {
      if (_retval == nullptr) {
        if (_type->name() == basic_type::TYPE_INT)
          _retval = new cdk::integer_node(this->lineno(), 0);
        else if (_type->name() == basic_type::TYPE_POINTER)
          _retval = new null_node(this->lineno());
      }
    }

  public:
    inline function_node(int lineno, int scope, basic_type *type,
        const char *name, cdk::sequence_node *arguments,
        block_node *body, cdk::expression_node *retval = nullptr)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(name),
        _arguments(arguments), _body(body), _retval(retval) {
      check_retval();
    }
    inline function_node(int lineno, int scope, basic_type *type,
        const std::string &name, cdk::sequence_node *arguments,
        block_node *body, cdk::expression_node *retval = nullptr)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(name),
        _arguments(arguments), _body(body), _retval(retval) {
      check_retval();
    }
    inline function_node(int lineno, int scope, basic_type *type,
        const std::string *name, cdk::sequence_node *arguments,
        block_node *body, cdk::expression_node *retval = nullptr)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(*name),
        _arguments(arguments), _body(body), _retval(retval) {
      check_retval();
    }

    ~function_node() {
      if (_retval) delete _retval;
    }

  public:
    inline const int scope() const {
      return _scope;
    }
    inline basic_type *type() {
      return _type;
    }
    inline const std::string &name() const {
      return _name;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }
    inline block_node *body() {
      return _body;
    }
    inline cdk::expression_node *retval() {
      return _retval;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_node(this, level);
    }

  };

} // xpl

#endif