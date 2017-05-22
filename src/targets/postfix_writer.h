#ifndef __XPL_SEMANTICS_POSTFIX_WRITER_H__
#define __XPL_SEMANTICS_POSTFIX_WRITER_H__

#include <iostream>
#include <set>
#include <stack>
#include <string>

#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace xpl {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<xpl::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

    // offset counters
    int _local_offset;
    int _args_offset;

    // useful for when visiting literals for globals
    bool _is_var_public;
    std::string _label; 

    // current function (nullptr means we're in the global context)
    std::shared_ptr<xpl::symbol> _curr_function;

    // keep track of segments
    enum segment {
      NONE, BSS, DATA, RODATA, TEXT
    };

    segment _old_segment;
    segment _curr_segment;

    // set of externs
    std::set<std::string> *_externs;

    // label stacks for nexts and stops
    std::stack<int> *_next_labels;
    std::stack<int> *_stop_labels;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<xpl::symbol> &symtab,
        cdk::basic_postfix_emitter &pf)
        : basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0),
        _local_offset(0), _args_offset(0), _is_var_public(false),
        _curr_function(nullptr), _old_segment(NONE), _curr_segment(NONE) {
      _externs = new std::set<std::string>();
      _next_labels = new std::stack<int>();
      _stop_labels = new std::stack<int>();
    }

  public:
    ~postfix_writer() {
      os().flush();
      delete _externs;
      delete _next_labels;
      delete _stop_labels;
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

    inline void change_segment(segment new_segment) {
      if (_curr_segment == new_segment) return; // do nothing
      
      switch (new_segment) {
        case BSS:    _pf.BSS();    break;
        case DATA:   _pf.DATA();   break;
        case RODATA: _pf.RODATA(); break;
        case TEXT:   _pf.TEXT();   break;
        default:
          throw std::string("Failed to change segment, shouldn't have happened");
      }
      _curr_segment = new_segment;

      if (_old_segment == NONE)
        _old_segment = _curr_segment;
    }

    inline void previous_segment() {
      change_segment(_old_segment);
    }

  public:
    inline void register_extern(const std::string &name) {
      _externs->insert(name);
    }
    
    void declare_externs() {
      // just in case, register these anyway
      _externs->insert("argc");
      _externs->insert("argv");
      _externs->insert("envp");
      for (const std::string &lbl : *_externs)
        _pf.EXTERN(lbl);
    }

  public:
    void do_sequence_node(cdk::sequence_node * const node, int lvl);

  protected:
    void processBinaryLogicalExpression(cdk::binary_expression_node * const node, int lvl);

  public:
    void do_integer_node(cdk::integer_node * const node, int lvl);
    void do_double_node(cdk::double_node * const node, int lvl);
    void do_string_node(cdk::string_node * const node, int lvl);

  public:
    void do_neg_node(cdk::neg_node * const node, int lvl);
    void do_not_node(cdk::not_node * const node, int lvl);

  public:
    void do_add_node(cdk::add_node * const node, int lvl);
    void do_sub_node(cdk::sub_node * const node, int lvl);
    void do_mul_node(cdk::mul_node * const node, int lvl);
    void do_div_node(cdk::div_node * const node, int lvl);
    void do_mod_node(cdk::mod_node * const node, int lvl);
    void do_lt_node(cdk::lt_node * const node, int lvl);
    void do_le_node(cdk::le_node * const node, int lvl);
    void do_ge_node(cdk::ge_node * const node, int lvl);
    void do_gt_node(cdk::gt_node * const node, int lvl);
    void do_ne_node(cdk::ne_node * const node, int lvl);
    void do_eq_node(cdk::eq_node * const node, int lvl);
    void do_and_node(cdk::and_node * const node, int lvl);
    void do_or_node(cdk::or_node * const node, int lvl);

  public:
    void do_identifier_node(cdk::identifier_node * const node, int lvl);
    void do_rvalue_node(cdk::rvalue_node * const node, int lvl);
    void do_assignment_node(cdk::assignment_node * const node, int lvl);

  public:
    void do_function_node(xpl::function_node * const node, int lvl);
    void do_var_node(xpl::var_node * const node, int lvl);
    void do_evaluation_node(xpl::evaluation_node * const node, int lvl);
    void do_print_node(xpl::print_node * const node, int lvl);
    void do_read_node(xpl::read_node * const node, int lvl);

  public:
    void do_while_node(xpl::while_node * const node, int lvl);
    void do_sweep_up_node(xpl::sweep_up_node * const node, int lvl);
    void do_sweep_down_node(xpl::sweep_down_node * const node, int lvl);
    void do_if_node(xpl::if_node * const node, int lvl);
    void do_if_else_node(xpl::if_else_node * const node, int lvl);

  public:
    void do_block_node(xpl::block_node * const node, int lvl);
    void do_next_node(xpl::next_node * const node, int lvl);
    void do_stop_node(xpl::stop_node * const node, int lvl);
    void do_vardecl_node(xpl::vardecl_node * const node, int lvl);
    void do_fundecl_node(xpl::fundecl_node * const node, int lvl);
    void do_return_node(xpl::return_node * const node, int lvl);

  public:
    void do_malloc_node(xpl::malloc_node * const node, int lvl);

  public:
    void do_index_node(xpl::index_node * const node, int lvl);

  public:
    void do_null_node(xpl::null_node * const node, int lvl);

  public:
    void do_identity_node(xpl::identity_node * const node, int lvl);
    void do_funcall_node(xpl::funcall_node * const node, int lvl);
    void do_address_of_node(xpl::address_of_node * const node, int lvl);

  };

} // xpl

#endif
