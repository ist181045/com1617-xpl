// $Id: vardecl_node.h,v 1.6 2017/04/13 11:10:23 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_VARDECLNODE_H__
#define __XPL_VARDECLNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/rvalue_node.h>
#include <cdk/basic_type.h>

#include <string>

namespace xpl {

  class vardecl_node: public cdk::basic_node {
    int _scope;
    basic_type *_type;
    std::string *_name;
    cdk::rvalue_node *_val;

  public:
    inline vardecl_node(int lineno, basic_type *type, std::string *name,
        cdk::rvalue_node *val)
        : cdk::basic_node(lineno), _type(type), _name(name), _val(val) {
    }

  public:
    inline const int scope() const {
      return _scope;
    }
    inline basic_type *type() {
      return _type;
    }
    inline std::string *name() {
      return _name;
    }
    inline cdk::rvalue_node *val() {
      return _val;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_vardecl_node(this, level);
    }

  };

} // xpl

#endif