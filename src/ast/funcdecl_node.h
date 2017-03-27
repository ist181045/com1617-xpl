// $Id: funcdecl_node.h,v 1.2 2017/03/27 01:58:02 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_FUNCDECLNODE_H__
#define __XPL_FUNCDECLNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>

namespace xpl {

  class funcdecl_node : public cdk::basic_node {
    int _scope;
    basic_type *_type;
    cdk::identifier_node *_name;
    cdk::sequence_node *_arguments;

  public:
    inline funcdecl_node(int lineno, basic_type *type,
        cdk::identifier_node *name, cdk::sequence_node *arguments)
        : cdk::basic_node(lineno), _type(type), _name(name),
        _arguments(arguments) {}

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

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_funcdecl_node(this, level);
    }

  };

} // xpl

#endif