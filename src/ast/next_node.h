// $Id: next_node.h,v 1.4 2017/04/11 22:36:30 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_NEXTNODE_H__
#define __XPL_NEXTNODE_H__

#include <cdk/ast/basic_node.h>

namespace xpl {

  class next_node: public cdk::basic_node {
  public:
    inline next_node(int lineno)
        : cdk::basic_node(lineno) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_next_node(this, level);
    }

  };

} // xpl

#endif
