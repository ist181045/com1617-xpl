// $Id: real_node.h,v 1.1 2017/03/24 12:11:26 ist181045 Exp $
#ifndef __XPL_REALNODE_H__
#define __XPL_REALNODE_H__

#include <cdk/ast/literal_node.h>

namespace xpl {
  class real_node : cdk::literal_node<double> {
  protected:
    inline real_node(int lineno, double d)
        : cdk::literal_node<double>(lineno, d) {}

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_real_node(this, level);
    }

  };
} // xpl

#endif