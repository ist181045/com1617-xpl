// $Id: null_node.h,v 1.3 2017/03/27 01:58:02 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_NULLNODE_H__
#define __XPL_NULLNODE_H__

#include <cdk/ast/literal_node.h>

namespace xpl {

  class null_node : public virtual cdk::literal_node<int> {
  public:
    inline null_node(int lineno) :
        cdk::literal_node<int>(lineno, 0) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_null_node(this, level);
    }

  };

} // xpl

#endif
