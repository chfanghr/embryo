//
// Created by 方泓睿 on 2020/4/15.
//

#include <embryo/compiler/code_gen/cg_block.h>
#include <embryo/compiler/code_gen/cg_stat.h>
#include <embryo/compiler/code_gen/cg_exp.h>
#include <embryo/compiler/code_gen/cg_helpers.h>

namespace embryo::code_gen {
void CGBlock(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::Block> &block) {
  for (const auto &stat:block->stats_)
    CGStat(fi, stat);

  if (!block->ret_exps_.empty())
    CGRetStat(fi, block->ret_exps_, block->last_line_);
}

void CGRetStat(const std::shared_ptr<FuncInfo> &fi,
               std::vector<std::shared_ptr<ast::Exp>> &exps, int32_t last_line) {
  auto n_exps = exps.size();
  if (n_exps == 0) {
    fi->EmitReturn(0, 0, last_line);
    return;
  }

  if (n_exps == 1) {
    auto name_exp = std::dynamic_pointer_cast<ast::NameExp>(exps[0]);
    if (name_exp) {
      auto r = fi->SlotOfLocVar(name_exp->name_);
      if (r >= 0)
        fi->EmitReturn(last_line, r, 1);
      return;
    }

    auto fc_exp = std::dynamic_pointer_cast<ast::FuncCallExp>(exps[0]);
    if (fc_exp) {
      auto r = fi->AllocReg();
      CGTailCallExp(fi, fc_exp, r);
      fi->FreeReg();
      fi->EmitReturn(last_line, r, -1);
      return;
    }
  }

  auto mult_ret = IsVarargOrFuncCall(exps[n_exps - 1]);
  for (size_t i = 0; i < exps.size(); i++) {
    auto exp = exps[i];
    auto r = fi->AllocReg();

    if (i == n_exps - 1 && mult_ret)
      CGExp(fi, exp, r, -1);
    else
      CGExp(fi, exp, r, 1);
  }

  fi->FreeRegs(n_exps);

  auto a = fi->used_regs_;
  if (mult_ret)
    fi->EmitReturn(last_line, a, -1);
  else
    fi->EmitReturn(last_line, a, n_exps);
}
}