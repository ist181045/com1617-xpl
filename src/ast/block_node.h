// $Id: block_node.h,v 1.1 2017/03/23 21:17:33 ist181045 Exp $
#ifndef __XPL_BLOCKNODE_H__
#define __XPL_BLOCKNODE_H__

#include <cdk/ast/basic_node.h>

namespace xpl {

  class block_node : cdk::basic_node {
    cdk::sequence_node *_declarations;
    cdk::sequence_node *_instructions;

  protected:
    inline block_node(int lineno, cdk::sequence_node *declarations,
        cdk::sequence_node *instructions) : cdk::basic_node(lineno),
        _declarations(declarations), _instructions(instructions) {}

  public:
    inline cdk::sequence_node *declarations() {
      return _declarations;
    }

    inline cdk::sequence_node *instructions() {
      return _instructions;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_block_node(this, level);
    }

  };

} // xpl

#endif