// $Id: type_checker.cpp,v 1.44 2017/05/22 14:42:19 ist181045 Exp $ -*- c++ -*-
#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

#define ASSERT_UNSPEC \
    { if (node->type() != nullptr && \
          node->type()->name() != basic_type::TYPE_UNSPEC) return; }

/** Handy helpers */
/* Creates and returns a basic_type* from a given type */
inline basic_type * create_type(basic_type * const type) {
  basic_type * new_type = new basic_type(type->size(), type->name());
  if (type->subtype())
    new_type->_subtype = create_type(type->subtype());
  return new_type;
}

/* If the given type is unspecified, default it to int */
inline void default_if_unspec(basic_type * const type) {
  if (type->name() == basic_type::TYPE_UNSPEC) {
    type->_size = 4;
    type->_name = basic_type::TYPE_INT;
  }
}

/*
 * If both are unspec, default to int. If one is unspec, take the type of the
 * other.
 */
void default_if_unspec(basic_type * const ltype, basic_type * const rtype) {
  if (ltype->name() == basic_type::TYPE_POINTER
      && rtype->name() == basic_type::TYPE_POINTER) {
    default_if_unspec(ltype->subtype(), rtype->subtype());
  }

  if (ltype->name() == basic_type::TYPE_UNSPEC
      && rtype->name() == basic_type::TYPE_UNSPEC) {
    ltype->_size = rtype->_size = 4;
    ltype->_name = rtype->_name = basic_type::TYPE_INT;
  } else if (ltype->name() == basic_type::TYPE_UNSPEC) {
    ltype->_size = rtype->size();
    ltype->_name = rtype->name();
  } else if (rtype->name() == basic_type::TYPE_UNSPEC) {
    rtype->_size = ltype->size();
    rtype->_name = ltype->size();
  }
}

/* Converts an arithmetic expression's type if they differ. */
inline void numeric_conversion(cdk::binary_expression_node * const node) {
  basic_type *ltype = node->left()->type();
  basic_type *rtype = node->right()->type();

  // delete type, will be overwritten
  if (!node->type()) {
    delete node->type();
  }
  
  if (ltype->name() == rtype->name()) {
    node->type(create_type(ltype));
  } else if ((ltype->name() == basic_type::TYPE_DOUBLE && rtype->name() == basic_type::TYPE_INT)
      || (ltype->name() == basic_type::TYPE_INT && rtype->name() == basic_type::TYPE_DOUBLE)) {
    node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
  } else {
    throw std::string("incompatible types in '"
      + node->label().substr(0, node->label().find("_node"))
      + "' expression");
  }
}

/* Checks if the two given types are matching pointer types. */
bool matching_pointers(basic_type * const ltype, basic_type * const rtype) {
  if (ltype->subtype() && rtype->subtype()) {
    return matching_pointers(ltype->subtype(), rtype->subtype());
  } else {
    return ltype->name() == rtype->name()
      && ltype->subtype() == rtype->subtype();
  }
}

/*
 * Checks if non commutative binary expression is compatible. Used in
 * assignments, decls, and so forth.
 */
bool check_compatible(basic_type * const target, basic_type * const other) {
  if (target->name() == other->name()) {
    if (target->name() == basic_type::TYPE_POINTER) {
      return matching_pointers(target, other);
    }
    return true;
  }

  return target->name() == basic_type::TYPE_DOUBLE
    && other->name() == basic_type::TYPE_INT;
}

/* Extra check for if other is a null_node. */
bool check_compatible(cdk::expression_node * const target, cdk::expression_node * const other) {
  if (!check_compatible(target->type(), other->type())
      && target->type()->name() == basic_type::TYPE_POINTER) {
    return dynamic_cast<xpl::null_node*>(other);
  }
  return false;
}

/* Checks and processes the types of two expression nodes. */
inline void xpl::type_checker::check_types(cdk::expression_node * const left, cdk::expression_node * const right, int lvl) {
  left->accept(this, lvl + 2);
  switch (left->type()->name()) {
    case basic_type::TYPE_INT:
    case basic_type::TYPE_DOUBLE:
    case basic_type::TYPE_POINTER:
      break;
    default:
      throw std::string("wrong type in left argument of '"
        + left->label().substr(0, left->label().find("_node"))
        + "' expression");
  }

  right->accept(this, lvl + 2);
  switch (right->type()->name()) {
    case basic_type::TYPE_INT:
    case basic_type::TYPE_DOUBLE:
    case basic_type::TYPE_POINTER:
      break;
    default:
      throw std::string("wrong type in right argument of '"
        + right->label().substr(0, right->label().find("_node"))
        + "' expression");
  }

  default_if_unspec(left->type(), right->type());
}

//===========================================================================

void xpl::type_checker::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t ix = 0; ix < node->size(); ++ix)
    node->node(ix)->accept(this, lvl);
}

//===========================================================================

void xpl::type_checker::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(8, basic_type::TYPE_DOUBLE));
}

void xpl::type_checker::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void xpl::type_checker::do_null_node(xpl::null_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_POINTER));
}

void xpl::type_checker::do_string_node(cdk::string_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type(4, basic_type::TYPE_STRING));
}

//===========================================================================

inline void xpl::type_checker::processUnaryExpression(cdk::unary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  default_if_unspec(node->argument()->type());
  
  switch (node->argument()->type()->name()) {
    case basic_type::TYPE_INT:
    case basic_type::TYPE_DOUBLE:
      break;
    default:
      throw std::string("wrong type in argument of '"
        + node->label().substr(0, node->label().find("_node"))
        + "' expression");
  }
  
  node->type(create_type(node->argument()->type()));
}

void xpl::type_checker::do_identity_node(xpl::identity_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void xpl::type_checker::do_neg_node(cdk::neg_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void xpl::type_checker::do_not_node(cdk::not_node * const node, int lvl) {
  processUnaryExpression(node, lvl);
  if (node->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in argument of 'not' expression, expected "\
      "int");
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

//---------------------------------------------------------------------------

void xpl::type_checker::do_address_of_node(xpl::address_of_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  node->type(new basic_type(4, basic_type::TYPE_POINTER));
  node->type()->_subtype = create_type(node->argument()->type());
}

void xpl::type_checker::do_malloc_node(xpl::malloc_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  if (node->expression()->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in argument of malloc expression, expected "\
      "int");

  node->type(new basic_type(4, basic_type::TYPE_POINTER));
  // context dependent
  node->type()->_subtype = new basic_type();
}

//===========================================================================

void xpl::type_checker::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;
  check_types(node->left(), node->right(), lvl);
}

inline void xpl::type_checker::processBinaryArithmeticExpression(cdk::binary_expression_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}

void xpl::type_checker::do_add_node(cdk::add_node * const node, int lvl) {
  processBinaryArithmeticExpression(node, lvl);

  // additional pointer behavior restrictions
  basic_type *ltype = node->left()->type();
  basic_type *rtype = node->right()->type();

  if (ltype->name() == basic_type::TYPE_POINTER
      && rtype->name() == basic_type::TYPE_INT) {
    node->type(create_type(ltype));
  } else if (ltype->name() == basic_type::TYPE_INT
      && rtype->name() == basic_type::TYPE_POINTER) {
    node->type(create_type(rtype));
  } else numeric_conversion(node);
}

void xpl::type_checker::do_sub_node(cdk::sub_node * const node, int lvl) {
  processBinaryArithmeticExpression(node, lvl);

  // additional pointer behavior restrictions
  basic_type *ltype = node->left()->type();
  basic_type *rtype = node->right()->type();

  if (ltype->name() == basic_type::TYPE_POINTER
      && rtype->name() == basic_type::TYPE_INT) {
    node->type(create_type(ltype));
  } else if (ltype->name() == basic_type::TYPE_POINTER
      && rtype->name() == basic_type::TYPE_POINTER) {
    if (matching_pointers(ltype, rtype)) {
      node->type(new basic_type(4, basic_type::TYPE_INT));
    } else {
      throw std::string("incompatible pointer types in sub expression");
    }
  } else numeric_conversion(node);
}

void xpl::type_checker::do_mul_node(cdk::mul_node * const node, int lvl) {
  processBinaryArithmeticExpression(node, lvl);
  if (node->left()->type()->name() == basic_type::TYPE_POINTER
      || node->right()->type()->name() == basic_type::TYPE_POINTER)
    throw std::string("incompatible types, division doesn't support pointer "\
      "operands");
  numeric_conversion(node);
}

void xpl::type_checker::do_div_node(cdk::div_node * const node, int lvl) {
  processBinaryArithmeticExpression(node, lvl);
  if (node->left()->type()->name() == basic_type::TYPE_POINTER
      || node->right()->type()->name() == basic_type::TYPE_POINTER)
    throw std::string("incompatible types, multiplication doesn't support "\
      "pointer operands");
  numeric_conversion(node);
}

void xpl::type_checker::do_mod_node(cdk::mod_node * const node, int lvl) {
  processBinaryArithmeticExpression(node, lvl);
  if (node->left()->type()->name() != basic_type::TYPE_INT
      || node->left()->type()->name() != basic_type::TYPE_INT)
    throw std::string("incompatible types, modulo (%%) only supports integer "\
      "operands");
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

//---------------------------------------------------------------------------

inline void xpl::type_checker::processBinaryComparisonExpression(cdk::binary_expression_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if (node->left()->type()->name() == basic_type::TYPE_POINTER
      || node->right()->type()->name() == basic_type::TYPE_POINTER)
    throw std::string("incompatible types in '"
      + node->label().substr(0, node->label().find("_node"))
      + "' expression, pointers unsupported");
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void xpl::type_checker::do_lt_node(cdk::lt_node * const node, int lvl) {
  processBinaryComparisonExpression(node, lvl);
}

void xpl::type_checker::do_le_node(cdk::le_node * const node, int lvl) {
  processBinaryComparisonExpression(node, lvl);
}

void xpl::type_checker::do_ge_node(cdk::ge_node * const node, int lvl) {
  processBinaryComparisonExpression(node, lvl);
}

void xpl::type_checker::do_gt_node(cdk::gt_node * const node, int lvl) {
  processBinaryComparisonExpression(node, lvl);
}

//---------------------------------------------------------------------------

inline void xpl::type_checker::processBinaryEqualityExpression(cdk::binary_expression_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void xpl::type_checker::do_eq_node(cdk::eq_node * const node, int lvl) {
  processBinaryEqualityExpression(node, lvl);
}

void xpl::type_checker::do_ne_node(cdk::ne_node * const node, int lvl) {
  processBinaryEqualityExpression(node, lvl);
}

//---------------------------------------------------------------------------

inline void xpl::type_checker::processBinaryLogicalExpression(cdk::binary_expression_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
  if (node->left()->type()->name() != basic_type::TYPE_INT
      || node->right()->type()->name() != basic_type::TYPE_INT)
    throw std::string("incompatible types in '"
      + node->label().substr(0, node->label().find("_node"))
      + "' expression, expected integers");
  node->type(new basic_type(4, basic_type::TYPE_INT));
}

void xpl::type_checker::do_and_node(cdk::and_node * const node, int lvl) {
  processBinaryLogicalExpression(node, lvl);
}

void xpl::type_checker::do_or_node(cdk::or_node * const node, int lvl) {
  processBinaryLogicalExpression(node, lvl);
}

//===========================================================================

void xpl::type_checker::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_UNSPEC;

  try {
    node->lvalue()->accept(this, lvl + 2);
  } catch (const std::string &id) {
    throw std::string(id + " was never declared");
  }
  node->rvalue()->accept(this, lvl + 2);

  if (node->lvalue()->type()->name() == basic_type::TYPE_STRING
      || node->rvalue()->type()->name() == basic_type::TYPE_STRING) {
    default_if_unspec(node->lvalue()->type(), node->rvalue()->type());
    if (node->lvalue()->type()->name() != node->rvalue()->type()->name())
      throw std::string("mismatching types in assignment expression");
  } else {
    check_types(node->lvalue(), node->rvalue(), lvl);
  }

  if (check_compatible(node->lvalue(), node->rvalue()))
    node->type(create_type(node->lvalue()->type()));
  else
    throw std::string("incompatible types in assignment expression");
}

void xpl::type_checker::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "'" + id + "' was never declared";
  }
}

void xpl::type_checker::do_evaluation_node(xpl::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  default_if_unspec(node->argument()->type());
}

//===========================================================================

void xpl::type_checker::do_print_node(xpl::print_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  default_if_unspec(node->argument()->type());
}

void xpl::type_checker::do_read_node(xpl::read_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(new basic_type());
}

//===========================================================================

void xpl::type_checker::do_sweep_up_node(xpl::sweep_up_node * const node, int lvl) {
  node->lvalue()->accept(this, lvl + 2);
  node->initial()->accept(this, lvl + 2);
  auto *assign = new cdk::assignment_node(node->lineno(), node->lvalue(), node->initial());
  assign->accept(this, lvl);

  node->upper()->accept(this, lvl + 2);
  cdk::le_node le(node->lineno(), node->lvalue(), node->upper());
  le.accept(this, lvl);

  node->step()->accept(this, lvl + 2);
  if (!check_compatible(node->lvalue(), node->step()))
    throw std::string("incompatible types in 'sweep+' statement's increment");
}

void xpl::type_checker::do_sweep_down_node(xpl::sweep_down_node * const node, int lvl) {
  node->lvalue()->accept(this, lvl + 2);
  node->initial()->accept(this, lvl + 2);
  auto *assign = new cdk::assignment_node(node->lineno(), node->lvalue(), node->initial());
  assign->accept(this, lvl);

  node->lower()->accept(this, lvl + 2);
  cdk::ge_node ge(node->lineno(), node->lvalue(), node->lower());
  ge.accept(this, lvl);

  node->step()->accept(this, lvl + 2);  
  if(!check_compatible(node->lvalue(), node->step()))
    throw std::string("incompatible types in 'sweep-' statement's decrement");
}

//===========================================================================

void xpl::type_checker::do_if_node(xpl::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  default_if_unspec(node->condition()->type());
}

void xpl::type_checker::do_if_else_node(xpl::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  default_if_unspec(node->condition()->type());
}

void xpl::type_checker::do_while_node(xpl::while_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  default_if_unspec(node->condition()->type());
}

//===========================================================================

void xpl::type_checker::do_index_node(xpl::index_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  if (node->expression()->type()->name() != basic_type::TYPE_POINTER
      || !node->expression()->type()->subtype())
    throw std::string("wrong type in index expression, expected a valid "\
      "pointer");

  node->offset()->accept(this, lvl + 2);
  if (node->offset()->type()->name() != basic_type::TYPE_INT)
    throw std::string("wrong type in index offset, expected int");

  node->type(create_type(node->expression()->type()->subtype()));
}

//===========================================================================

void xpl::type_checker::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<xpl::symbol> symbol = _symtab.find(id);

  if (!symbol)
    throw id;
  else
    node->type(symbol->type());
}

void xpl::type_checker::do_var_node(xpl::var_node * const node, int lvl) {
  const std::string &id = node->name();
  std::shared_ptr<xpl::symbol> symbol = _symtab.find_local(id);
  if (symbol) {
    throw "'" + id + "' was already declared";
  }

  node->value()->accept(this, lvl);
  if (!check_compatible(node->type(), node->value()->type())) {
    if (node->type()->name() == basic_type::TYPE_POINTER
        && !dynamic_cast<xpl::null_node*>(node->value()))
      throw std::string("incompatible types in variable definition");
  }
  symbol = std::make_shared<xpl::symbol>(node->type(), node->name(),
    node->scope(), false, true);
  _symtab.insert(id, symbol);
}

void xpl::type_checker::do_fundecl_node(xpl::fundecl_node * const node, int lvl) {
  const std::string &id = node->name();
  std::shared_ptr<xpl::symbol> symbol = _symtab.find(id);
  if (symbol)
    throw "'" + id + "' was already declared";
  
  symbol = std::make_shared<xpl::symbol>(node->type(), node->name(),
    node->scope(), true, false, 0, true, node->arguments());
  _symtab.insert(id, symbol);
  
  _symtab.push();
  _symtab.insert(id, symbol);
  node->arguments()->accept(this, lvl + 2);
  _symtab.pop();
}

void xpl::type_checker::do_function_node(xpl::function_node * const node, int lvl) {
  const std::string &id = node->name();
  std::shared_ptr<xpl::symbol> symbol = _symtab.find(id);
  if (symbol) {
    if (symbol->isdefined())
      throw "'" + id + "' was alread defined";
    else if (symbol->scope() == 2)
      throw "Can't define external function '" + id + "'";
  }

  symbol = std::make_shared<xpl::symbol>(node->type(), node->name(),
    node->scope(), true, false, 0, true, node->arguments());
  _symtab.insert(id, symbol);
  
  _symtab.push();
  _symtab.insert(id, symbol);
  node->arguments()->accept(this, lvl + 2);
  for (size_t ix = 0; ix < node->arguments()->size(); ++ix) {
    xpl::vardecl_node *var =
      dynamic_cast<xpl::vardecl_node*>(node->arguments()->node(ix));
    symbol = std::make_shared<xpl::symbol>(var->type(), var->name(), 0);
    _symtab.insert(var->name(), symbol);
  }

  if (node->retval()) {
    node->retval()->accept(this, lvl);
    if (!check_compatible(node->type(), node->retval()->type())) {
      if (node->type()->name() == basic_type::TYPE_POINTER
          && !dynamic_cast<xpl::null_node*>(node->retval()))
        throw std::string("incompatible types in variable definition");
    }
  }
  _symtab.pop();
  _symtab.pop();
}

void xpl::type_checker::do_funcall_node(xpl::funcall_node * const node, int lvl) {
  const std::string &id = node->name();
  std::shared_ptr<xpl::symbol> symbol = _symtab.find(id);
  if (!symbol)
    throw "'" + id + "' function wasn't declared";
  else if (!symbol->isfunction())
    throw "'" + id + "' is not a function";
  else if (!symbol->isdefined() && symbol->scope() != 2)
    throw "'" + id + "' function wasn't defined";

  cdk::sequence_node *params = symbol->params();
  cdk::sequence_node *args = node->arguments();
  if (params) {
    if (!args)
      throw std::string("missing arguments");
    else if (params->size() != args->size())
      throw std::string("number of arguments differ");
  } else if (args) {
    throw "'" + id + "' doesn't take any arguments";
  }

  args->accept(this, lvl);
  for (size_t ix = 0; ix < args->size(); ++ix) {
    cdk::expression_node *arg =
      dynamic_cast<cdk::expression_node*>(args->node(ix));
    xpl::vardecl_node *param =
      dynamic_cast<xpl::vardecl_node*>(params->node(ix));

    if (!check_compatible(arg->type(), param->type())) {
      if (param->type()->name() == basic_type::TYPE_POINTER
          && !dynamic_cast<xpl::null_node*>(arg))
        throw std::string("incompatible types");
    }
  }
}
