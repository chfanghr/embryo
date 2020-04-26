//
// Created by 方泓睿 on 2020/4/4.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_AST_EXP_H_
#define EMBRYO_INCLUDE_EMBRYO_AST_EXP_H_

#include <cstdlib>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <cstdint>

#include <embryo/compiler/ast/common.h>
#include <embryo/compiler/lexer/token.h>

namespace embryo::ast {
class Block;

// exp ::= nil | false | true

class NilExp : public Exp {
 public:
  size_t line_;

  [[nodiscard]] std::string String() const override;

  explicit NilExp(size_t line) : line_(line) {}
};

class TrueExp : public Exp {
 public:
  size_t line_;

  [[nodiscard]] std::string String() const override;

  explicit TrueExp(size_t line) : line_(line) {}
};

class FalseExp : public Exp {
 public:
  size_t line_;

  [[nodiscard]] std::string String() const override;

  explicit FalseExp(size_t line) : line_(line) {}
};

// exp ::= ‘...’
class VarArgExp : public Exp {
 public:
  size_t line_;

  [[nodiscard]] std::string String() const override;

  explicit VarArgExp(size_t line) : line_(line) {}
};

// exp ::= Numeral

class IntegerExp : public Exp {
 public:
  size_t line_;
  int64_t val_;

  [[nodiscard]] std::string String() const override;

  IntegerExp(size_t line, int64_t val) : line_(line), val_(val) {}
};

class FloatExp : public Exp {
 public:
  size_t line_;
  double val_;

  [[nodiscard]] std::string String() const override;

  FloatExp(size_t line, double val) : line_(line), val_(val) {}
};

// exp ::= LiteralString
class StringExp : public Exp {
 public:
  size_t line_;
  std::string str_;

  [[nodiscard]] std::string String() const override;

  StringExp(size_t line, std::string str)
      : line_(line), str_(std::move(str)) {}
};

class NameExp : public Exp {
 public:
  size_t line_;
  std::string name_;

  [[nodiscard]] std::string String() const override;
  NameExp(size_t line, std::string name)
      : line_(line), name_(std::move(name)) {}
};

// exp ::= unop exp
class UnOpExp : public Exp {
 public:
  size_t line_;
  lexer::Token op_; // operator
  std::shared_ptr<Exp> exp_;

  [[nodiscard]] std::string String() const override;
  UnOpExp(size_t line, lexer::Token op, std::shared_ptr<Exp> exp)
      : line_(line), op_(op), exp_(std::move(exp)) {}
};

// exp ::= exp binop exp
class BinOpExp : public Exp {
 public:
  size_t line_;
  lexer::Token op_; // operator
  std::shared_ptr<Exp> exp_1_, exp_2_;

  [[nodiscard]] std::string String() const override;

  BinOpExp(size_t line, lexer::Token op, std::shared_ptr<Exp> exp_1, std::shared_ptr<Exp> exp_2)
      : line_(line), op_(op), exp_1_(std::move(exp_1)), exp_2_(std::move(exp_2)) {}
};

class ConcatExp : public Exp {
 public:
  size_t line_;
  std::vector<std::shared_ptr<Exp>> exps_;

  [[nodiscard]] std::string String() const override;

  ConcatExp(size_t line, std::vector<std::shared_ptr<Exp>> exp)
      : line_(line), exps_(std::move(exp)) {}
};

// tableconstructor ::= ‘{’ [fieldlist] ‘}’
// fieldlist ::= field {fieldsep field} [fieldsep]
// field ::= ‘[’ exp ‘]’ ‘=’ exp | Name ‘=’ exp | exp
// fieldsep ::= ‘,’ | ‘;’
class TableConstructorExp : public Exp {
 public:
  size_t line_;
  size_t last_line_;
  std::vector<std::shared_ptr<Exp>> key_exps_, val_exps_;

  [[nodiscard]] std::string String() const override;

  TableConstructorExp(size_t line,
                      size_t last_line,
                      std::vector<std::shared_ptr<Exp>> key_exp,
                      std::vector<std::shared_ptr<Exp>> val_exp)
      : line_(line), last_line_(last_line), key_exps_(std::move(key_exp)), val_exps_(std::move(val_exp)) {}
};

// functiondef ::= function funcbody
// funcbody ::= ‘(’ [parlist] ‘)’ block end
// parlist ::= namelist [‘,’ ‘...’] | ‘...’
// namelist ::= Name {‘,’ Name}
class FuncDefExp : public Exp {
 public:
  size_t line_{};
  size_t last_line_{}; // line of "end"
  std::vector<std::string> par_list_{};
  bool is_var_arg_{};
  std::shared_ptr<Block> block_{};

  [[nodiscard]] std::string String() const override;

  FuncDefExp(size_t line,
             size_t last_line,
             std::vector<std::string> par_list,
             bool is_var_arg,
             std::shared_ptr<Block> block)
      : line_(line),
        last_line_(last_line),
        par_list_(std::move(par_list)),
        is_var_arg_(is_var_arg),
        block_(std::move(block)) {}

  FuncDefExp(size_t last_line, bool is_var_arg, std::shared_ptr<Block> block)
      : last_line_(last_line), is_var_arg_(is_var_arg), block_(std::move(block)) {}

  FuncDefExp(size_t line, size_t last_line, bool is_var_arg, std::shared_ptr<Block> block)
      : line_(line), last_line_(last_line), is_var_arg_(is_var_arg), block_(std::move(block)) {}
};

// prefixexp ::= var | functioncall | ‘(’ exp ‘)’

class ParensExp : public Exp {
 public:
  std::shared_ptr<Exp> exp_;

  [[nodiscard]] std::string String() const override;

  explicit ParensExp(std::shared_ptr<Exp> exp) : exp_(std::move(exp)) {}
};

class TableAccessExp : public Exp {
 public:
  size_t last_line_;
  std::shared_ptr<Exp> prefix_exp_, key_exp_;

  [[nodiscard]] std::string String() const override;

  TableAccessExp(size_t last_line, std::shared_ptr<Exp> prefix_exp, std::shared_ptr<Exp> key_exp)
      : last_line_(last_line), prefix_exp_(std::move(prefix_exp)), key_exp_(std::move(key_exp)) {}
};

// functioncall ::= prefixexp ‘:’ Name args
// args ::= ‘(’ [explist] ‘)’
// args ::= tableconstructor
// args ::= LiteralString
class FuncCallExp : public Exp {
 public:
  size_t line_; // line of "("
  size_t last_line_; // line of ")"
  std::shared_ptr<Exp> prefix_exp_;
  std::shared_ptr<StringExp> name_exp_;
  std::vector<std::shared_ptr<Exp>> args_;

  [[nodiscard]] std::string String() const override;

  FuncCallExp(size_t line,
              size_t last_line,
              std::shared_ptr<Exp> prefix_exp,
              std::shared_ptr<StringExp> name_exp,
              std::vector<std::shared_ptr<Exp>> args)
      : line_(line),
        last_line_(last_line),
        prefix_exp_(std::move(prefix_exp)),
        name_exp_(std::move(name_exp)),
        args_(std::move(args)) {}
};
}

#endif //EMBRYO_INCLUDE_EMBRYO_AST_EXP_H_
