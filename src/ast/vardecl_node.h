#ifndef __XPL_VARDECLNODE_H__
#define __XPL_VARDECLNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>

#include <string>

namespace xpl {

  class vardecl_node: public cdk::basic_node {
    int _scope;
    basic_type *_type;
    std::string _name;

  public:
    inline vardecl_node(int lineno, int scope, basic_type *type,
        const char *name)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(name) {
    }
    inline vardecl_node(int lineno, int scope, basic_type *type,
        const std::string &name)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(name) {
    }
    inline vardecl_node(int lineno, int scope, basic_type *type,
        const std::string *name)
        : cdk::basic_node(lineno), _scope(scope), _type(type), _name(*name) {
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

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_vardecl_node(this, level);
    }

  };

} // xpl

#endif