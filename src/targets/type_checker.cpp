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
    throw std::string("incompatible types in binary expression");
  }
}

/* Checks if the two given types are matching pointer types. */
bool matching_pointers(basic_type * const ltype, basic_type * const rtype) {
  if (ltype->subtype() && rtype->subtype()) {
    return matching_pointers(ltype->subtype(), rtype->subtype());
  } else {
    return ltype->name() == rtype->name();
  }
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
  node->type()->_subtype = new basic_type(0, basic_type::TYPE_UNSPEC);
}

//===========================================================================

void xpl::type_checker::processBinaryExpression(cdk::binary_expression_node * const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  switch (node->left()->type()->name()) {
    case basic_type::TYPE_INT:
    case basic_type::TYPE_DOUBLE:
    case basic_type::TYPE_POINTER:
      break;
    default:
      throw std::string("wrong type in left argument of '"
        + node->label().substr(0, node->label().find("_node"))
        + "' expression");
  }

  node->right()->accept(this, lvl + 2);
  switch (node->right()->type()->name()) {
    case basic_type::TYPE_INT:
    case basic_type::TYPE_DOUBLE:
    case basic_type::TYPE_POINTER:
      break;
    default:
      throw std::string("wrong type in right argument of '"
        + node->label().substr(0, node->label().find("_node"))
        + "' expression");
  }

  default_if_unspec(node->left()->type(), node->right()->type());
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
      node->type(create_type(ltype));
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

void xpl::type_checker::do_and_node(cdk::and_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void xpl::type_checker::do_or_node(cdk::or_node * const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void xpl::type_checker::do_identifier_node(cdk::identifier_node * const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<xpl::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  }
  else {
    throw id;
  }
}

void xpl::type_checker::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_UNSPEC;

  try {
    node->lvalue()->accept(this, lvl);
  } catch (const std::string &id) {
    basic_type *type = node->lvalue()->type();
    std::shared_ptr<xpl::symbol> symbol =
      std::make_shared<xpl::symbol>(new basic_type(type->size(), type->name()), id, 0);
    _symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
    node->lvalue()->accept(this, lvl);  //DAVID: bah!
  }

  node->rvalue()->accept(this, lvl + 2);
  if (node->lvalue()->type()->name() != node->rvalue()->type()->name())
    throw std::string("mismatching types in arguments of assignment expression");

  // in XPL, expressions are always int
  node->type(new basic_type(
    node->lvalue()->type()->size(), node->lvalue()->type()->name()));
}

//---------------------------------------------------------------------------

void xpl::type_checker::do_function_node(xpl::function_node * const node, int lvl) {}
void xpl::type_checker::do_var_node(xpl::var_node * const node, int lvl) {}

void xpl::type_checker::do_evaluation_node(xpl::evaluation_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void xpl::type_checker::do_print_node(xpl::print_node * const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void xpl::type_checker::do_read_node(xpl::read_node * const node, int lvl) {
  // dunno
}

//---------------------------------------------------------------------------

void xpl::type_checker::do_while_node(xpl::while_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}
void xpl::type_checker::do_sweep_up_node(xpl::sweep_up_node * const node, int lvl) {
  node->lvalue()->accept(this, lvl + 2);
  node->initial()->accept(this, lvl + 2);
  node->upper()->accept(this, lvl + 2);
  node->step()->accept(this, lvl + 2);
}
void xpl::type_checker::do_sweep_down_node(xpl::sweep_down_node * const node, int lvl) {
  node->lvalue()->accept(this, lvl + 2);
  node->initial()->accept(this, lvl + 2);
  node->lower()->accept(this, lvl + 2);
  node->step()->accept(this, lvl + 2);  
}

//---------------------------------------------------------------------------

void xpl::type_checker::do_if_node(xpl::if_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void xpl::type_checker::do_if_else_node(xpl::if_else_node * const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void xpl::type_checker::do_block_node(xpl::block_node * const node, int lvl) {
  _symtab.push();
  node->declarations()->accept(this, lvl + 2);
  node->statements()->accept(this, lvl + 2);
  _symtab.pop();
}
void xpl::type_checker::do_vardecl_node(xpl::vardecl_node * const node, int lvl) {}
void xpl::type_checker::do_fundecl_node(xpl::fundecl_node * const node, int lvl) {}

//---------------------------------------------------------------------------

void xpl::type_checker::do_index_node(xpl::index_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  node->offset()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void xpl::type_checker::do_funcall_node(xpl::funcall_node * const node, int lvl) {}
