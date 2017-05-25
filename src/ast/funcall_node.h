#ifndef __XPL_FUNCALLNODE_H__
#define __XPL_FUNCALLNODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

#include <string>

namespace xpl {

  class funcall_node: public cdk::expression_node {
    std::string _name;
    cdk::sequence_node *_arguments;

  public:
    inline funcall_node(int lineno, const char *name,
        cdk::sequence_node *arguments)
        : cdk::expression_node(lineno), _name(name),
        _arguments(arguments) {
    }
    inline funcall_node(int lineno, const std::string &name,
        cdk::sequence_node *arguments)
        : cdk::expression_node(lineno), _name(name),
        _arguments(arguments) {
    }
    inline funcall_node(int lineno, const std::string *name,
        cdk::sequence_node *arguments)
        : cdk::expression_node(lineno), _name(*name),
        _arguments(arguments) {
    }

  public:
    inline const std::string &name() const {
      return _name;
    }
    inline cdk::sequence_node *arguments() {
      return _arguments;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_funcall_node(this, level);
    }

  };
  
} // xpl

#endif