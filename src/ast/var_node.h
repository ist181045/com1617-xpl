// $Id: var_node.h,v 1.3 2017/05/17 17:06:44 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_VARNODE_H__
#define __XPL_VARNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/basic_type.h>

#include <string>

namespace xpl {

  class var_node: public cdk::basic_node {
    int _scope;
    basic_type *_type;
    std::string _name;
    cdk::expression_node *_value;

  public:
    inline var_node(int lineno, int scope, basic_type *type,
        const char *name, cdk::expression_node *value)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(name),
        _value(value) {
    }
    inline var_node(int lineno, int scope, basic_type *type,
        const std::string &name, cdk::expression_node *value)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(name),
        _value(value) {
    }
    inline var_node(int lineno, int scope, basic_type *type,
        const std::string *name, cdk::expression_node *value)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(*name),
        _value(value) {
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
    inline cdk::expression_node *value() {
      return _value;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_var_node(this, level);
    }

  };

} // xpl

#endif