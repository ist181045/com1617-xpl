// $Id: return_node.h,v 1.3 2017/03/27 01:58:02 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_RETURNNODE_H__
#define __XPL_RETURNNODE_H__

#include <cdk/ast/basic_node.h>

namespace xpl {

  class return_node : public cdk::basic_node {
  public:
    inline return_node(int lineno) :
        cdk::basic_node(lineno) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_return_node(this, level);
    }

  };

} // xpl

#endif
