// $Id: var_node.h,v 1.1 2017/04/17 15:35:35 ist181045 Exp $ -*- c++ -*-
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
    std::string *_name;
    cdk::expression_node *_val;

  public:
    inline var_node(int lineno, int scope, basic_type *type,
        std::string *name, cdk::expression_node *val)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(name),
        _val(val) {
    }

  public:
    inline const int scope() const {
      return _scope;
    }
    inline basic_type *type() {
      return _type;
    }
    inline const std::string &name() const {
      return *_name;
    }
    inline cdk::expression_node *val() {
      return _val;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_var_node(this, level);
    }

  };

} // xpl

#endif