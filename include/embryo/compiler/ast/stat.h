//
// Created by 方泓睿 on 2020/4/4.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_AST_STAT_H_
#define EMBRYO_INCLUDE_EMBRYO_AST_STAT_H_

#include <cstdlib>
#include <string>
#include <memory>
#include <utility>
#include <vector>

#include <embryo/compiler/ast/common.h>

namespace embryo::ast {

// stat ::= ‘;’
class EmptyStat : public Stat {
 public:
  [[nodiscard]] std::string String() const override;
};

// break
class BreakStat : public Stat {
 public:
  [[nodiscard]] std::string String() const override;

  size_t line_;

  explicit BreakStat(size_t line) : line_(line) {}
};

// stat ::= label
// label ::= ‘::’ Name ‘::’
class LabelStat : public Stat {
 public:
  std::string name_;

  [[nodiscard]] std::string String() const override;

  explicit LabelStat(std::string name) : name_(std::move(name)) {}
};

// stat ::= goto Name
class GotoStat : public Stat {
 public:
  std::string name_;

  [[nodiscard]] std::string String() const override;

  explicit GotoStat(std::string name) : name_(std::move(name)) {}
};

// stat ::= do block end
class DoStat : public Stat {
 public:
  std::shared_ptr<Block> block_;

  [[nodiscard]] std::string String() const override;

  explicit DoStat(std::shared_ptr<Block> block) : block_(std::move(block)) {}
};

// stat ::= while exp do block end
class WhileStat : public Stat {
 public:
  std::shared_ptr<Exp> exp_;
  std::shared_ptr<Block> block_;

  [[nodiscard]] std::string String() const override;

  WhileStat(std::shared_ptr<Exp> exp, std::shared_ptr<Block> block)
      : exp_(std::move(exp)), block_(std::move(block)) {}
};

// stat ::= repeat block until exp
class RepeatStat : public Stat {
 public:
  std::shared_ptr<Exp> exp_;
  std::shared_ptr<Block> block_;

  [[nodiscard]] std::string String() const override;

  RepeatStat(std::shared_ptr<Exp> exp, std::shared_ptr<Block> block)
      : exp_(std::move(exp)), block_(std::move(block)) {}
};

// stat ::= if exp then block {elseif exp then block} [else block] end ==>
// stat ::= if exp then block {elseif exp then block} [elseif true block] end ==>
// stat ::= if exp then block {elseif exp then block} end
class IfStat : public Stat {
 public:
  std::vector<std::shared_ptr<Exp>> exps_;
  std::vector<std::shared_ptr<Block>> block_;

  [[nodiscard]] std::string String() const override;

  IfStat(std::vector<std::shared_ptr<Exp>> exp, std::vector<std::shared_ptr<Block>> block)
      : exps_(std::move(exp)), block_(std::move(block)) {}
};

// stat ::= for Name ‘=’ exp ‘,’ exp [‘,’ exp] do block end
class ForNumStat : public Stat {
 public:
  size_t line_of_for_;
  size_t line_of_do_;
  std::string var_name_;
  std::shared_ptr<Exp> init_exp_, limit_exp_, step_exp_;
  std::shared_ptr<Block> block_;

  [[nodiscard]] std::string String() const override;

  ForNumStat(size_t line_of_for,
             size_t line_of_do,
             std::string var_name,
             std::shared_ptr<Exp> init_exp,
             std::shared_ptr<Exp> limit_exp,
             std::shared_ptr<Exp> step_exp,
             std::shared_ptr<Block> block)
      : line_of_for_(line_of_for),
        line_of_do_(line_of_do),
        var_name_(std::move(var_name)),
        init_exp_(std::move(init_exp)),
        limit_exp_(std::move(limit_exp)),
        step_exp_(std::move(step_exp)),
        block_(std::move(block)) {}
};

// stat ::= for namelist in explist do block end
// namelist ::= Name {‘,’ Name}
class ForInStat : public Stat {
 public:
  size_t line_of_do_;
  std::vector<std::string> name_list_;
  std::vector<std::shared_ptr<Exp>> exp_list_;
  std::shared_ptr<Block> block_;

  [[nodiscard]] std::string String() const override;

  ForInStat(size_t line_of_do,
            std::vector<std::string> name_list,
            std::vector<std::shared_ptr<Exp>> exp_list,
            std::shared_ptr<Block> block)
      : line_of_do_(line_of_do),
        name_list_(std::move(name_list)),
        exp_list_(std::move(exp_list)),
        block_(std::move(block)) {}
};

// stat ::= local namelist [‘=’ explist]
class LocalVarDeclStat : public Stat {
 public:
  size_t last_line_;
  std::vector<std::string> name_list_;
  std::vector<std::shared_ptr<Exp>> exp_list_;

  [[nodiscard]] std::string String() const override;

  LocalVarDeclStat(size_t last_line,
                   std::vector<std::string> name_list,
                   std::vector<std::shared_ptr<Exp>> exp_list)
      : last_line_(last_line), name_list_(std::move(name_list)), exp_list_(std::move(exp_list)) {}
};

// stat ::= varlist ‘=’ explist
// varlist ::= var {‘,’ var}
// explist ::= exp {‘,’ exp}
class AssignStat : public Stat {
 public:
  size_t last_line_;
  std::vector<std::shared_ptr<Exp>> var_list_;
  std::vector<std::shared_ptr<Exp>> exp_list_;

  [[nodiscard]] std::string String() const override;

  AssignStat(size_t last_line,
             std::vector<std::shared_ptr<Exp>> var_list,
             std::vector<std::shared_ptr<Exp>> exp_list)
      : last_line_(last_line), var_list_(std::move(var_list)), exp_list_(std::move(exp_list)) {}
};

class FuncDefExp;

// stat ::= local function Name funcbody
// funcname ::= Name {‘.’ Name} [‘:’ Name]
class LocalFuncDefStat : public Stat {
 public:
  std::string name_;
  std::shared_ptr<FuncDefExp> func_def_exp_;

  [[nodiscard]] std::string String() const override;

  LocalFuncDefStat(std::string name, std::shared_ptr<FuncDefExp> func_def_exp)
      : name_(std::move(name)), func_def_exp_(std::move(func_def_exp)) {}
};
}

#endif //EMBRYO_INCLUDE_EMBRYO_AST_STAT_H_
