// $Id: program_node.h,v 1.1 2017/02/17 16:02:31 david Exp $ -*- c++ -*-
#ifndef __XPL_PROGRAMNODE_H__
#define __XPL_PROGRAMNODE_H__

#include <cdk/ast/basic_node.h>

namespace xpl {

  /**
   * Class for describing program nodes.
   */
  class program_node: public cdk::basic_node {
    cdk::basic_node *_statements;

  public:
    inline program_node(int lineno, cdk::basic_node *statements) :
        cdk::basic_node(lineno), _statements(statements) {
    }

  public:
    inline cdk::basic_node *statements() {
      return _statements;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_program_node(this, level);
    }

  };

} // xpl

#endif
