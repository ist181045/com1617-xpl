// $Id: symbol.h,v 1.3 2017/05/21 16:04:34 ist181045 Exp $ -*- c++ -*-
#ifndef __XPL_SEMANTICS_SYMBOL_H__
#define __XPL_SEMANTICS_SYMBOL_H__

#include <cdk/basic_type.h>

#include <string>

// forward declaration of sequence_node
namespace cdk { class sequence_node; }
namespace xpl {

    class symbol {
      basic_type *_type;
      std::string _name;
      int _scope;

      bool _is_global;
      bool _is_defined;
      int _offset;

      bool _is_function;
      cdk::sequence_node *_params;

    public:
      inline symbol(basic_type *type, const std::string &name, int scope = 0,
          bool is_global = false, bool is_defined = false, int offset = 0,
          bool is_function = false, cdk::sequence_node * params = nullptr)
          : _type(type), _name(name), _offset(offset), _scope(scope),
          _is_global(is_global), _is_defined(is_defined),
          _is_function(is_function), _params(params) {
      }

      virtual ~symbol() {
        delete _type;
      }

      inline basic_type *type() const {
        return _type;
      }
      inline const std::string &name() const {
        return _name;
      }
      inline int scope() const {
        return _scope;
      }
      inline void scope(int scope) {
        _scope = scope;
      }

      inline bool isglobal() const {
        return _is_global;
      }
      inline void isglobal(bool is_global) {
        _is_global = is_global;
      }
      inline bool isdefined() const {
        return _is_defined;
      }
      inline void isdefined(bool is_defined) {
        _is_defined = is_defined;
      }
      inline int offset() const {
        return _offset;
      }
      inline void offset(int offset) {
        _offset = offset;
      }
      
      inline bool isfunction() const {
        return _is_function;
      }
      inline void isfunction(bool is_function) {
        _is_function = is_function;
      }
      inline cdk::sequence_node * params() const {
        return _params;
      }
      inline void params(cdk::sequence_node * params) {
        _params = params;
      }
    };

} // xpl

#endif
