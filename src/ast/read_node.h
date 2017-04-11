// $Id: read_node.h,v 1.3 2017/04/11 22:36:30 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_READNODE_H__
#define __XPL_READNODE_H__

#include <cdk/ast/lvalue_node.h>

namespace xpl {

  class read_node: public cdk::expression_node {
  public:
    inline read_node(int lineno)
        : cdk::expression_node(lineno) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_read_node(this, level);
    }

  };

} // xpl

#endif
