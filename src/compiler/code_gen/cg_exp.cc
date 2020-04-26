//
// Created by 方泓睿 on 2020/4/16.
//

#include <embryo/utils/inspection.h>
#include <embryo/compiler/code_gen/cg_exp.h>
#include <embryo/compiler/code_gen/cg_block.h>
#include <embryo/compiler/code_gen/cg_helpers.h>

namespace embryo::code_gen {
namespace {
int32_t PrepFuncCall(const std::shared_ptr<FuncInfo> &fi,
                     const std::shared_ptr<ast::FuncCallExp> &exp,
                     int32_t a) {
  auto n_args = exp->args_.size();
  auto last_arg_is_var_arg_or_func_call = false;

  CGExp(fi, exp->prefix_exp_, a, 1);
  if (exp->name_exp_) {
    fi->AllocReg();
    auto[c, k]=ExpToOpArg(fi, exp->name_exp_, ArgKind::kRK);
    fi->EmitSelf(exp->line_, a, a, c);
    if (k == ArgKind::kReg)
      fi->FreeRegs(1);
  }

  for (size_t i = 0; i < exp->args_.size(); ++i) {
    auto arg = exp->args_[i];
    auto tmp = fi->AllocReg();
    if (i == n_args - 1 && IsVarargOrFuncCall(arg)) {
      last_arg_is_var_arg_or_func_call = true;
      CGExp(fi, arg, tmp, -1);
    } else
      CGExp(fi, arg, tmp, 1);
  }
  fi->FreeRegs(n_args);

  if (exp->name_exp_) {
    fi->FreeReg();
    ++n_args;
  }

  if (last_arg_is_var_arg_or_func_call)
    n_args = -1;

  return n_args;
}
}

void CGExp(const std::shared_ptr<FuncInfo> &fi,
           const std::shared_ptr<ast::Exp> &exp,
           int32_t a, int32_t n) {
  {
    auto ptr = std::dynamic_pointer_cast<ast::NilExp>(exp);
    if (ptr) {
      fi->EmitLoadNil(ptr->line_, a, n);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FalseExp>(exp);
    if (ptr) {
      fi->EmitLoadBool(ptr->line_, a, 0, 0);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TrueExp>(exp);
    if (ptr) {
      fi->EmitLoadBool(ptr->line_, a, 1, 0);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::IntegerExp>(exp);
    if (ptr) {
      fi->EmitLoadK(ptr->line_, a, ptr->val_);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FloatExp>(exp);
    if (ptr) {
      fi->EmitLoadK(ptr->line_, a, ptr->val_);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::StringExp>(exp);
    if (ptr) {
      fi->EmitLoadK(ptr->line_, a, ptr->str_);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::ParensExp>(exp);
    if (ptr) {
      CGExp(fi, ptr->exp_, a, 1);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::VarArgExp>(exp);
    if (ptr) {
      CGVarArgExp(fi, ptr, a, n);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FuncDefExp>(exp);
    if (ptr) {
      CGFuncDefExp(fi, ptr, a);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TableConstructorExp>(exp);
    if (ptr) {
      CGTableConstructorExp(fi, ptr, a);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::UnOpExp>(exp);
    if (ptr) {
      CGUnOpExp(fi, ptr, a);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::BinOpExp>(exp);
    if (ptr) {
      CGBinOpExp(fi, ptr, a);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::ConcatExp>(exp);
    if (ptr) {
      CGConcatExp(fi, ptr, a);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::NameExp>(exp);
    if (ptr) {
      CGNameExp(fi, ptr, a);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::TableAccessExp>(exp);
    if (ptr) {
      CGTableAccessExp(fi, ptr, a);
      return;
    }
  }
  {
    auto ptr = std::dynamic_pointer_cast<ast::FuncCallExp>(exp);
    if (ptr) {
      CGFuncCallExp(fi, ptr, a, n);
      return;
    }
  }
}

std::tuple<int32_t, ArgKind> ExpToOpArg(const std::shared_ptr<FuncInfo> &fi,
                                        const std::shared_ptr<ast::Exp> &exp,
                                        ArgKind args_kinds) {
  if ((static_cast<int32_t>(args_kinds) & // NOLINT(hicpp-signed-bitwise)
      static_cast< int32_t>(ArgKind::kConst)) > 0) { // NOLINT(hicpp-signed-bitwise)
    int32_t idx = -1;
    {
      auto ptr = std::dynamic_pointer_cast<ast::NilExp>(exp);
      if (ptr)
        idx = fi->IndexOfConstant(nullptr);
    }
    {
      auto ptr = std::dynamic_pointer_cast<ast::TrueExp>(exp);
      if (ptr)
        idx = fi->IndexOfConstant(true);
    }
    {
      auto ptr = std::dynamic_pointer_cast<ast::FalseExp>(exp);
      if (ptr)
        idx = fi->IndexOfConstant(false);
    }
    {
      auto ptr = std::dynamic_pointer_cast<ast::IntegerExp>(exp);
      if (ptr)
        idx = fi->IndexOfConstant(ptr->val_);
    }
    {
      auto ptr = std::dynamic_pointer_cast<ast::FloatExp>(exp);
      if (ptr)
        idx = fi->IndexOfConstant(ptr->val_);
    }
    {
      auto ptr = std::dynamic_pointer_cast<ast::StringExp>(exp);
      if (ptr)
        idx = fi->IndexOfConstant(ptr->str_);
    }
    if (idx >= 0 && idx <= 0xFF)
      return {0x100 + idx, ArgKind::kConst};
  }

  auto name_exp = std::dynamic_pointer_cast<ast::NameExp>(exp);
  if (name_exp) {
    if ((static_cast<int32_t>(args_kinds) & // NOLINT(hicpp-signed-bitwise)
        static_cast< int32_t>(ArgKind::kReg)) > 0) { // NOLINT(hicpp-signed-bitwise)
      auto r = fi->SlotOfLocVar(name_exp->name_);
      if (r >= 0)
        return {r, ArgKind::kReg};
    }
    if ((static_cast<int32_t>(args_kinds) & // NOLINT(hicpp-signed-bitwise)
        static_cast< int32_t>(ArgKind::kUpVal)) > 0) { // NOLINT(hicpp-signed-bitwise)
      auto idx = fi->IndexOfUpVal(name_exp->name_);
      if (idx >= 0)
        return {idx, ArgKind::kUpVal};
    }
  }

  auto a = fi->AllocReg();
  CGExp(fi, exp, a, 1);
  return {a, ArgKind::kReg};
}

void CGTailCallExp(const std::shared_ptr<FuncInfo> &fi,
                   const std::shared_ptr<ast::FuncCallExp> &exp,
                   int32_t a) {
  auto n_args = PrepFuncCall(fi, exp, a);
  fi->EmitTailCall(exp->line_, a, n_args);
}

void CGFuncDefExp(const std::shared_ptr<FuncInfo> &fi,
                  const std::shared_ptr<ast::FuncDefExp> &exp,
                  int32_t a) {
  auto sub_fi = std::make_shared<FuncInfo>(fi, exp);
  fi->sub_funcs_.push_back(sub_fi);

  for (const auto &param:exp->par_list_)
    sub_fi->AddLocVar(param, 0);

  CGBlock(sub_fi, exp->block_);
  sub_fi->ExitScope(sub_fi->PC() + 2);
  sub_fi->EmitReturn(exp->last_line_, 0, 0);

  auto bx = fi->sub_funcs_.size() - 1;
  fi->EmitClosure(exp->last_line_, a, bx);
}

void CGFuncCallExp(const std::shared_ptr<FuncInfo> &fi,
                   const std::shared_ptr<ast::FuncCallExp> &exp,
                   int32_t a,
                   int32_t n) {
  auto n_args = PrepFuncCall(fi, exp, a);
  fi->EmitCall(exp->line_, a, n_args, n);
}

void CGVarArgExp(const std::shared_ptr<FuncInfo> &fi,
                 const std::shared_ptr<ast::VarArgExp> &exp,
                 int32_t a,
                 int32_t n) {
  if (!fi->is_var_arg)
    throw std::runtime_error(MSG_WITH_FUNC_NAME("cannot use \"...\" outside a vararg function"));
  fi->EmitVarArg(exp->line_, a, n);
}

void CGTableConstructorExp(const std::shared_ptr<FuncInfo> &fi,
                           const std::shared_ptr<ast::TableConstructorExp> &exp,
                           int32_t a) {
  int32_t n_arr = 0;
  for (const auto &key_exp:exp->key_exps_) {
    if (!key_exp)
      ++n_arr;
  }

  auto n_exps = exp->key_exps_.size();
  auto mult_ret = n_exps > 0 && IsVarargOrFuncCall(exp->val_exps_[n_exps - 1]);

  fi->EmitNewTable(exp->line_, a, n_arr, n_exps - n_arr);

  int32_t arr_idx = 0;

  for (size_t i = 0; i < exp->key_exps_.size(); i++) {
    const auto &key_exp = exp->key_exps_[i];
    const auto &val_exp = exp->val_exps_[i];

    if (!key_exp) {
      ++arr_idx;
      auto tmp = fi->AllocReg();
      if (i == n_exps - 1 && mult_ret)
        CGExp(fi, val_exp, tmp, -1);
      else
        CGExp(fi, val_exp, tmp, 1);

      if (arr_idx % 50 == 0 || arr_idx == n_arr) {
        auto n = arr_idx % 50;
        if (n == 0)
          n = 50;

        fi->FreeRegs(n);
        auto line = LastLineOf(val_exp);
        auto c = (arr_idx - 1) / 50 + 1;

        if (i == n_exps - 1 && mult_ret)
          fi->EmitSetList(line, a, 0, c);
        else
          fi->EmitSetList(line, a, n, c);
      }

      continue;
    }

    auto b = fi->AllocReg();
    CGExp(fi, key_exp, b, 1);
    auto c = fi->AllocReg();
    CGExp(fi, val_exp, c, 1);
    fi->FreeRegs(2);

    auto line = LastLineOf(val_exp);
    fi->EmitSetTable(line, a, b, c);
  }
}

void CGUnOpExp(const std::shared_ptr<FuncInfo> &fi,
               const std::shared_ptr<ast::UnOpExp> &exp,
               int32_t a) {
  auto old_regs = fi->used_regs_;
  auto[b, b_type] =ExpToOpArg(fi, exp->exp_, ArgKind::kReg);
  fi->EmitUnaryOp(exp->line_, exp->op_, a, b);
  fi->used_regs_ = old_regs;
}

void CGBinOpExp(const std::shared_ptr<FuncInfo> &fi,
                const std::shared_ptr<ast::BinOpExp> &exp,
                int32_t a) {
  switch (exp->op_) {
    case lexer::Token::kOpAnd:
    case lexer::Token::kOpOr: {
      auto old_regs = fi->used_regs_;

      auto[b, b_type]=ExpToOpArg(fi, exp->exp_1_, ArgKind::kReg);
      fi->used_regs_ = old_regs;
      if (exp->op_ == lexer::Token::kOpAnd)
        fi->EmitTestSet(exp->line_, a, b, 0);
      else
        fi->EmitTestSet(exp->line_, a, b, 1);
      auto pc_of_jmp = fi->EmitJmp(exp->line_, 0, 0);

      std::tie(b, b_type) = ExpToOpArg(fi, exp->exp_2_, ArgKind::kReg);
      fi->used_regs_ = old_regs;
      fi->EmitMove(exp->line_, a, b);
      fi->FixSBx(pc_of_jmp, fi->PC() - pc_of_jmp);
    }
    default: {
      auto old_regs = fi->used_regs_;
      auto[b, b_type]=ExpToOpArg(fi, exp->exp_1_, ArgKind::kRK);
      auto[c, c_type]=ExpToOpArg(fi, exp->exp_2_, ArgKind::kRK);
      fi->EmitBinaryOp(exp->line_, exp->op_, a, b, c);
      fi->used_regs_ = old_regs;
    }
  }
}

void CGConcatExp(const std::shared_ptr<FuncInfo> &fi,
                 const std::shared_ptr<ast::ConcatExp> &exp,
                 int32_t a) {
  for (const auto &sub_exp:exp->exps_) {
    auto reg = fi->AllocReg();
    CGExp(fi, sub_exp, reg, 1);
  }

  auto c = fi->used_regs_ - 1;
  auto b = c - exp->exps_.size() + 1;
  fi->FreeRegs(c - b + 1);
  fi->EmitABC(exp->line_, vm::Op::kConcat, a, b, c);
}

void CGNameExp(const std::shared_ptr<FuncInfo> &fi,
               const std::shared_ptr<ast::NameExp> &exp,
               int32_t a) {
  auto r = fi->SlotOfLocVar(exp->name_);
  if (r >= 0) {
    fi->EmitMove(exp->line_, a, r);
    return;
  }

  auto idx = fi->IndexOfUpVal(exp->name_);
  if (idx >= 0) {
    fi->EmitGetUpVal(exp->line_, a, idx);
    return;
  }

  auto ta_exp = std::make_shared<ast::TableAccessExp>(
      exp->line_,
      std::make_shared<ast::NameExp>(exp->line_, "_ENV"),
      std::make_shared<ast::StringExp>(exp->line_, exp->name_));
  CGTableAccessExp(fi, ta_exp, a);
}

void CGTableAccessExp(const std::shared_ptr<FuncInfo> &fi,
                      const std::shared_ptr<ast::TableAccessExp> &exp,
                      int32_t a) {
  auto old_regs = fi->used_regs_;
  auto[b, b_kind]=ExpToOpArg(fi, exp->prefix_exp_, ArgKind::kRU);
  auto[c, c_kind] =ExpToOpArg(fi, exp->key_exp_, ArgKind::kRK);
  fi->used_regs_ = old_regs;

  if (b_kind == ArgKind::kUpVal)
    fi->EmitGetTabUp(exp->last_line_, a, b, c);
  else
    fi->EmitGetTable(exp->last_line_, a, b, c);
}

}