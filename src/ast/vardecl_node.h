// $Id: vardecl_node.h,v 1.10 2017/04/17 15:35:35 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_VARDECLNODE_H__
#define __XPL_VARDECLNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>

#include <string>

namespace xpl {

  class vardecl_node: public cdk::basic_node {
    int _scope;
    basic_type *_type;
    std::string *_name;

  public:
    inline vardecl_node(int lineno, int scope, basic_type *type,
        std::string *name)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(name) {
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

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_vardecl_node(this, level);
    }

  };

} // xpl

#endif