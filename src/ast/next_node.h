// $Id: next_node.h,v 1.2 2017/03/23 19:05:07 ist181045 Exp $
#ifndef __XPL_NEXTNODE_H__
#define __XPL_NEXTNODE_H__

#include <cdk/ast/basic_node.h>

namespace xpl {

  class next_node : public cdk::basic_node {
  protected:
    inline next_node(int lineno) :
        cdk::basic_node(lineno) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_next_node(this, level);
    }

  };

} // cdk

#endif
