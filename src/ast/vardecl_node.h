// $Id: vardecl_node.h,v 1.1 2017/03/24 11:06:43 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_VARDECLNODE_H__
#define __XPL_VARDECLNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>

namespace xpl {

  class vardecl_node : cdk::basic_node {
    basic_type *_type;
    cdk::identifier_node *_name;

  protected:
    inline vardecl_node(int lineno, basic_type *type,
        cdk::identifier_node *name) : cdk::basic_node(lineno), _type(type),
        _name(name) {}

  public:
    inline basic_type *type() {
      return _type;
    }

    inline cdk::identifier_node *name() {
      return _name;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_vardecl_node(this, level);
    }

  };

} // xpl

#endif