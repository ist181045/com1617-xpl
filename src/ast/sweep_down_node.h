// $Id: sweep_down_node.h,v 1.1 2017/03/23 20:51:07 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_SWEEPDOWNNODE_H__
#define __XPL_SWEEPDOWNNODE_H__

#include <cdk/ast/basic_node.h>

#include <cdk/ast/expression_node.h>
#include <cdk/ast/integer_node.h>
#include <cdk/ast/lvalue_node.h>

namespace xpl {

  class sweep_down_node : public cdk::basic_node {
    cdk::lvalue_node *_lvalue;
    cdk::expression_node *_initial;
    cdk::expression_node *_lower;
    cdk::expression_node *_decrement;
    cdk::basic_node *_block;

  protected:
    inline sweep_down_node(int lineno, cdk::lvalue_node *lvalue,
        cdk::expression_node *initial, cdk::expression_node *lower,
        cdk::expression_node *decrement, cdk::basic_node *block)
        : cdk::basic_node(lineno), _lvalue(lvalue), _initial(initial),
        _lower(lower), _decrement(decrement), _block(block) {
    }

  public:

    inline cdk::lvalue_node *lvalue() {
      return _lvalue;
    }

    inline cdk::expression_node *initial() {
      return _initial;
    }

    inline cdk::expression_node *lower() {
      return _lower;
    }

    inline cdk::expression_node *decrement() {
      return _decrement;
    }

    inline cdk::basic_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_sweep_down_node(this, level);
    }

  };

} // xpl

#endif
