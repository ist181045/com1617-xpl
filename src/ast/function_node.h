// $Id: function_node.h,v 1.4 2017/03/24 11:41:59 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_FUNCTIONNODE_H__
#define __XPL_FUNCTIONNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/basic_type.h>

namespace xpl {

  /**
   * Class for describing program nodes.
   */
  class function_node: public cdk::basic_node {
    basic_type *_type;
    block_node *_body;

  public:
    inline function_node(int lineno, basic_type *type, block_node *body) :
      cdk::basic_node(lineno), _type(type), _body(body) {
    }

  public:
    inline basic_type *type() {
      return _type;
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