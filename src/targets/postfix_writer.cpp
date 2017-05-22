#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "ast/all.h"  // all.h is automatically generated

//===========================================================================

void xpl::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//===========================================================================

void xpl::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl;

  // if we're not inside a function (global context)
  if (!_curr_function) {
    change_segment(DATA); // change segments
    _pf.ALIGN(); // align
    if (_is_var_public) // if public
      _pf.GLOBAL(_label, _pf.OBJ()); // make it global
    _pf.LABEL(_label); // label of the identifier
    _pf.DOUBLE(node->value()); // "store" value
    previous_segment(); // return
  } else {
    change_segment(RODATA); // change segments
    _pf.ALIGN(); // align
    if (_is_var_public)
      _pf.GLOBAL(mklbl(lbl = ++_lbl), _pf.OBJ()); // make it global
    _pf.LABEL(mklbl(lbl)); // give it a label
    _pf.DOUBLE(node->value()); // "store" value
    previous_segment(); // return

    _pf.ADDR(mklbl(lbl)); // reference it
    _pf.DLOAD(); // load its value onto the stack
  }
}

void xpl::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // if we're not inside a function (global context)
  if (!_curr_function) {
    change_segment(DATA); // change segments
    _pf.ALIGN(); // align
    if (_is_var_public) // if public
      _pf.GLOBAL(_label, _pf.OBJ()); // make it global
    _pf.LABEL(_label); // label of the identifier
    _pf.CONST(node->value()); // "store" value
    previous_segment(); // return
  } else {
    _pf.INT(node->value()); // push the integer on the stack
  }
}

void xpl::postfix_writer::do_null_node(xpl::null_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (!_curr_function) {
    int lbl;
    change_segment(DATA);
    _pf.ALIGN();
    if (_is_var_public)
      _pf.GLOBAL(mklbl(lbl = ++_lbl), _pf.OBJ());
    _pf.LABEL(mklbl(lbl));
    _pf.CONST(node->value());
    previous_segment();
  } else {
    _pf.INT(node->value());
  }
}

void xpl::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl;

  /* generate the string */
  change_segment(RODATA); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl = ++_lbl)); // give the string a name
  _pf.STR(node->value()); // output string characters
  previous_segment(); // return

  // if we're not inside a function (global context)
  if (!_curr_function) {
    change_segment(DATA); // change segments
    _pf.ALIGN(); // align
    if (_is_var_public) // if public
      _pf.GLOBAL(_label, _pf.OBJ()); // make it global
    _pf.LABEL(_label); // label of the identifier
    _pf.ID(mklbl(lbl)); // give it an id
    previous_segment(); // return
  } else {
    _pf.ADDR(mklbl(lbl)); // load a reference onto the stack
  }
}

//==========================================================================-

void xpl::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.INT(0);
  _pf.EQ();
}

void xpl::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

//===========================================================================

void xpl::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == basic_type::TYPE_POINTER
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.INT(node->type()->subtype()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->right()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == basic_type::TYPE_POINTER
      && node->right()->type()->name() == basic_type::TYPE_INT) {
    _pf.INT(node->type()->subtype()->size());
    _pf.MUL();
  }

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DADD();
  else
    _pf.ADD();
}

void xpl::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  } else if (node->type()->name() == basic_type::TYPE_POINTER
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.INT(node->type()->subtype()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  }

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DSUB();
  else
    _pf.SUB();

  if (node->left()->type()->name() == basic_type::TYPE_POINTER
      && node->right()->type()->name() == basic_type::TYPE_POINTER) {
    _pf.INT(node->left()->type()->subtype()->size());
    _pf.DIV();
  }
}

void xpl::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  }

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DMUL();
  else
    _pf.MUL();
}

void xpl::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->left()->type()->name() == basic_type::TYPE_INT) {
    _pf.I2D();
  }

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DDIV();
  else
    _pf.DIV();
}

void xpl::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

//---------------------------------------------------------------------------

/* Process most of the (both) binary logical (and comparison) binary expressions */
inline void xpl::postfix_writer::processBinaryLogicalExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  bool promote_to_double = false; // by default, no need for I2D

  node->left()->accept(this, lvl);
  if ((promote_to_double = node->left()->type()->name() == basic_type::TYPE_DOUBLE))
    _pf.I2D();

  node->right()->accept(this, lvl);
  if ((promote_to_double = node->right()->type()->name() == basic_type::TYPE_DOUBLE))
    _pf.I2D();

  if (promote_to_double) {
    _pf.DCMP();
    _pf.INT(0);
  }
}

void xpl::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  processBinaryLogicalExpression(node, lvl);
  _pf.LT();
}

void xpl::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  processBinaryLogicalExpression(node, lvl);
  _pf.LE();
}

void xpl::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  processBinaryLogicalExpression(node, lvl);
  _pf.GE();
}

void xpl::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  processBinaryLogicalExpression(node, lvl);
  _pf.GT();
}

void xpl::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  processBinaryLogicalExpression(node, lvl);
  _pf.NE();
}

void xpl::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  processBinaryLogicalExpression(node, lvl);
  _pf.EQ();
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl;

  node->left()->accept(this, lvl);
  _pf.JZ(mklbl(lbl = ++_lbl)); // if 0 (false), just jump over rhs
  node->right()->accept(this, lvl);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

void xpl::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl;

  node->left()->accept(this, lvl);
  _pf.JNZ(mklbl(lbl = ++_lbl)); // if not 0 (true), just jump over rhs
  node->right()->accept(this, lvl);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//===========================================================================

void xpl::postfix_writer::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  std::shared_ptr<xpl::symbol> symbol = _symtab.find(node->name());
  if (symbol) {
    if (!_curr_function)
      _pf.ADDR(symbol->name());
    else
      _pf.LOCAL(symbol->offset());
  } else {
    throw "'" + node->name() + "'' was never declared";
  }
}

void xpl::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl); // determine the new value
  if (node->rvalue()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DDUP();
  else
    _pf.DUP();

  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->rvalue()->type()->name() == basic_type::TYPE_INT)
    _pf.I2D();

  node->lvalue()->accept(this, lvl);
  if (node->type()->name() == basic_type::TYPE_DOUBLE
      && node->lvalue()->type()->name() == basic_type::TYPE_INT)
    _pf.I2D();

  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DSTORE();
  else
    _pf.STORE();
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_fundecl_node(xpl::fundecl_node * const node, int lvl) {}
void xpl::postfix_writer::do_function_node(xpl::function_node * const node, int lvl) {
  std::string id = node->name();
  std::shared_ptr<xpl::symbol> symbol = _symtab.find(id);
  if (id == "xpl") id = "_main";

  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL(id, _pf.FUNC());
  _pf.LABEL(id);
  
  cdk::sequence_node *decls = node->body()->declarations();
  int stack_size = 0;
  for (size_t ix = 0; ix < decls->size(); ++ix) {
    xpl::vardecl_node *decl = dynamic_cast<xpl::vardecl_node*>(decls->node(ix));
    xpl::var_node *var  = dynamic_cast<xpl::var_node*>(decls->node(ix));

    stack_size += var ? var->type()->size() : decl->type()->size();
  }
  _pf.ENTER(stack_size);

  node->body()->accept(this, lvl);

  // end the main function
  _pf.INT(0);
  _pf.POP();
  _pf.LEAVE();
  _pf.RET();
}

void xpl::postfix_writer::do_funcall_node(xpl::funcall_node * const node, int lvl) {}

void xpl::postfix_writer::do_vardecl_node(xpl::vardecl_node * const node, int lvl) {}
void xpl::postfix_writer::do_var_node(xpl::var_node * const node, int lvl) {
  node->value()->accept(this, lvl);
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_evaluation_node(xpl::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.TRASH(node->argument()->type()->size());
}

void xpl::postfix_writer::do_print_node(xpl::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string func;

  node->argument()->accept(this, lvl); // determine the value to print
  switch (node->argument()->type()->name()) {
    case basic_type::TYPE_INT:    func = "printi"; break;
    case basic_type::TYPE_DOUBLE: func = "printd"; break;
    case basic_type::TYPE_STRING: func = "prints"; break;
    default:
      std::cerr << "ERROR: CANNOT PRINT THAT" << std::endl;
      exit(1);
  }

  register_extern(func); // register function
  _pf.CALL(func); // call it
  _pf.TRASH(node->argument()->type()->size()); // trash return

  if (node->newline()) { // if a newline is to be printed
    register_extern("println"); // register additional function
    _pf.CALL("println"); // call it
  }
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_read_node(xpl::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string func;
  switch (node->type()->name()) {
    case basic_type::TYPE_INT:    func = "readi"; break;
    case basic_type::TYPE_DOUBLE: func = "readd"; break;
    default:
      std::cerr << "ERROR: CANNOT READ THAT!" << std::endl;
      exit(1);
  }

  register_extern(func);
  if (node->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DPUSH();
  else
    _pf.PUSH();
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_while_node(xpl::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl2 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2));
}

void xpl::postfix_writer::do_sweep_up_node(xpl::sweep_up_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2, lbl3;

  cdk::assignment_node assign(node->lineno(), node->lvalue(), node->initial());
  assign.accept(this, lvl);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1 = ++_lbl));

  cdk::le_node le(node->lineno(), node->lvalue(), node->upper());
  le.accept(this, lvl);
  _pf.JZ(mklbl(lbl3 = ++_lbl));

  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2 = ++_lbl));
  node->step()->accept(this, lvl);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl3));

  _next_labels->push(lbl2);
  _stop_labels->push(lbl3);

  node->block()->accept(this, lvl);
}

void xpl::postfix_writer::do_sweep_down_node(xpl::sweep_down_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2, lbl3;

  cdk::assignment_node assign(node->lineno(), node->lvalue(), node->initial());
  assign.accept(this, lvl);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl1 = ++_lbl));

  cdk::ge_node ge(node->lineno(), node->lvalue(), node->lower());
  ge.accept(this, lvl);
  _pf.JZ(mklbl(lbl3 = ++_lbl));

  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2 = ++_lbl));
  node->step()->accept(this, lvl);
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl3));

  _next_labels->push(lbl2);
  _stop_labels->push(lbl3);

  node->block()->accept(this, lvl);
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_if_node(xpl::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_if_else_node(xpl::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_block_node(xpl::block_node * const node, int lvl) {
  _symtab.push();
  node->declarations()->accept(this, lvl);
  node->statements()->accept(this, lvl);
  _symtab.pop();
}

void xpl::postfix_writer::do_next_node(xpl::next_node * const node, int lvl) {
  _pf.JMP(mklbl(_next_labels->top()));
}

void xpl::postfix_writer::do_stop_node(xpl::stop_node * const node, int lvl) {
  _pf.JMP(mklbl(_stop_labels->top()));
}

void xpl::postfix_writer::do_return_node(xpl::return_node * const node, int lvl) {
  _pf.JMP(_curr_function->name() + "ret");
}


//---------------------------------------------------------------------------

void xpl::postfix_writer::do_malloc_node(xpl::malloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->expression()->accept(this, lvl); // size
  _pf.INT(node->type()->subtype()->size()); // sizeof(subtype)
  _pf.MUL(); // size * sizeof(subtype)
  _pf.ALLOC(); // push size bytes onto the stack
  _pf.SP(); // push stack pointer address
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_index_node(xpl::index_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->expression()->accept(this, lvl); // &lval
  node->offset()->accept(this, lvl); // offset
  _pf.INT(node->expression()->type()->size()); // sizeof(type)
  _pf.MUL(); // offset * sizeof(type)
  _pf.ADD(); // &lval + offset * sizeof(type)
}

//---------------------------------------------------------------------------

void xpl::postfix_writer::do_identity_node(xpl::identity_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  if (node->argument()->type()->name() == basic_type::TYPE_DOUBLE)
    _pf.DDUP();
  else
    _pf.DUP();
}

void xpl::postfix_writer::do_address_of_node(xpl::address_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
}
