// $Id: null_node.h,v 1.1 2017/03/23 19:40:27 ist181045 Exp $
#ifndef __XPL_NULLNODE_H__
#define __XPL_NULLNODE_H__

#include <cdk/ast/literal_node.h>

namespace xpl {

  class null_node : public cdk::literal_node<int> {
  protected:
    inline null_node(int lineno) :
        cdk::literal_node<int>(lineno, 0) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_null_node(this, level);
    }

  };

} // cdk

#endif
