//
// Created by 方泓睿 on 2020/4/16.
//

#include <embryo/utils/inspection.h>
#include <embryo/compiler/code_gen/cg_helpers.h>

namespace embryo::code_gen {
bool IsVarargOrFuncCall(const std::shared_ptr<ast::Exp> &in) {
  return std::dynamic_pointer_cast<ast::VarArgExp>(in) ||
      std::dynamic_pointer_cast<ast::FuncCallExp>(in);
}

std::optional<std::vector<std::shared_ptr<ast::Exp>>> RemoveTailNils(const std::vector<std::shared_ptr<ast::Exp>> &exps) {
  for (int64_t n = exps.size() - 1; n >= 0; --n) {
    if (!std::dynamic_pointer_cast<ast::NilExp>(exps[n]))
      return std::vector<std::shared_ptr<ast::Exp>>(exps.begin(), exps.begin() + n + 1);
  }

  return {};
}

int32_t LineOf(const std::shared_ptr<ast::Exp> &exp) {
  {
    auto ptr = std::dynamic_pointer_cast<ast::NilExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TrueExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FalseExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::IntegerExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FloatExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::StringExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::VarArgExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::NameExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FuncDefExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FuncCallExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TableConstructorExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::UnOpExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TableAccessExp>(exp);
    if (ptr) return LineOf(ptr->prefix_exp_);
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::ConcatExp>(exp);
    if (ptr) return LineOf(ptr->exps_[0]);
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::BinOpExp>(exp);
    if (ptr) return LineOf(ptr->exp_1_);
  }

  throw std::runtime_error(MSG_WITH_FUNC_NAME("unknown expression type"));
}

int32_t LastLineOf(const std::shared_ptr<ast::Exp> &exp) {
  {
    auto ptr = std::dynamic_pointer_cast<ast::NilExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TrueExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FalseExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::IntegerExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FloatExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::StringExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::VarArgExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::NameExp>(exp);
    if (ptr) return ptr->line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FuncDefExp>(exp);
    if (ptr) return ptr->last_line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FuncCallExp>(exp);
    if (ptr) return ptr->last_line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TableConstructorExp>(exp);
    if (ptr) return ptr->last_line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::UnOpExp>(exp);
    if (ptr) return LastLineOf(ptr->exp_);
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TableAccessExp>(exp);
    if (ptr) return ptr->last_line_;
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::ConcatExp>(exp);
    if (ptr) return LastLineOf(ptr->exps_[ptr->exps_.size() - 1]);
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::BinOpExp>(exp);
    if (ptr) return LastLineOf(ptr->exp_2_);
  }

  throw std::runtime_error(MSG_WITH_FUNC_NAME("unknown expression type"));
}
}