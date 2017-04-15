// $Id: sweep_up_node.h,v 1.6 2017/04/15 11:56:50 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_SWEEPUPNODE_H__
#define __XPL_SWEEPUPNODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/integer_node.h>
#include <cdk/ast/lvalue_node.h>

namespace xpl {

  class sweep_up_node: public cdk::basic_node {
    cdk::lvalue_node *_lvalue;
    cdk::expression_node *_initial;
    cdk::expression_node *_upper;
    cdk::expression_node *_step;
    cdk::basic_node *_block;

  public:
    inline sweep_up_node(int lineno, cdk::lvalue_node *lvalue,
        cdk::expression_node *initial, cdk::expression_node *upper,
        cdk::basic_node *block, cdk::expression_node *step = nullptr)
        : cdk::basic_node(lineno), _lvalue(lvalue), _initial(initial),
        _upper(upper), _step(step), _block(block) {
      if (step == nullptr) _step = new cdk::integer_node(lineno, 1);
    }

    ~sweep_up_node() {
      if (_step) delete _step;
    }

  public:
    inline cdk::lvalue_node *lvalue() {
      return _lvalue;
    }
    inline cdk::expression_node *initial() {
      return _initial;
    }
    inline cdk::expression_node *upper() {
      return _upper;
    }
    inline cdk::expression_node *step() {
      return _step;
    }
    inline cdk::basic_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_sweep_up_node(this, level);
    }

  };

} // xpl

#endif
