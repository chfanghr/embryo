//
// Created by 方泓睿 on 2020/4/16.
//

#include <embryo/utils/inspection.h>
#include <embryo/compiler/code_gen/cg_stat.h>
#include <embryo/compiler/code_gen/cg_exp.h>
#include <embryo/compiler/code_gen/cg_block.h>
#include <embryo/compiler/code_gen/cg_helpers.h>

namespace embryo::code_gen {
namespace {
template<typename T>
bool Try(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::Stat> &stat,
         const std::function<void(const std::shared_ptr<FuncInfo> &, const std::shared_ptr<T> &)> &func) {
  if (std::dynamic_pointer_cast<T>(stat)) {
    func(fi, std::dynamic_pointer_cast<T>(stat));
    return true;
  }
  return false;
}
}

void CGStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::Stat> &stat) {
  if (Try<ast::FuncCallExp>(fi, stat, CGFuncCallStat) ||
      Try<ast::BreakStat>(fi, stat, CGBreakStat) ||
      Try<ast::LocalFuncDefStat>(fi, stat, CGLocalFuncDefStat) ||
      Try<ast::DoStat>(fi, stat, CGDoStat) ||
      Try<ast::RepeatStat>(fi, stat, CGRepeatStat) ||
      Try<ast::WhileStat>(fi, stat, CGWhileStat) ||
      Try<ast::IfStat>(fi, stat, CGIfStat) ||
      Try<ast::ForNumStat>(fi, stat, CGForNumStat) ||
      Try<ast::ForInStat>(fi, stat, CGForInStat) ||
      Try<ast::AssignStat>(fi, stat, CGAssignStat) ||
      Try<ast::LocalVarDeclStat>(fi, stat, CGLocalVarDeclStat) ||
      Try<ast::LabelStat>(fi, stat, CGLabelStat) ||
      Try<ast::GotoStat>(fi, stat, CGGotoStat))
    return;

  throw std::runtime_error(MSG_WITH_FUNC_NAME("unsupported statement"));
}

void CGBreakStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::BreakStat> &stat) {
  auto pc = fi->EmitJmp(stat->line_, 0, 0);
  fi->AddBreakJmp(pc);
}

void CGLocalFuncDefStat(const std::shared_ptr<FuncInfo> &fi,
                        const std::shared_ptr<ast::LocalFuncDefStat> &stat) {
  CGFuncDefExp(fi, stat->func_def_exp_, fi->AddLocVar(stat->name_, fi->PC() + 2));
}

void CGFuncCallStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::FuncCallExp> &stat) {
  CGFuncCallExp(fi, stat, fi->AllocReg(), 0);
  fi->FreeReg();
}

void CGDoStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::DoStat> &stat) {
  fi->EnterScope(false);
  CGBlock(fi, stat->block_);
  fi->CloseOpenUpVals(stat->block_->last_line_);
  fi->ExitScope(fi->PC() + 1);
}

/*
         ______________
        |  false? jmp  |
        V              /
 repeat block until exp

*/
void CGRepeatStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::RepeatStat> &stat) {
  fi->EnterScope(true);
  auto pc_before_block = fi->PC();
  CGBlock(fi, stat->block_);

  auto old_regs = fi->used_regs_;
  auto[arg, arg_kink]=ExpToOpArg(fi, stat->exp_, ArgKind::kReg);
  fi->used_regs_ = old_regs;

  auto line = LastLineOf(stat->exp_);
  fi->EmitTest(line, arg, 0);
  fi->EmitJmp(line, fi->GetJmpArgA(), pc_before_block - fi->PC() - 1);
  fi->CloseOpenUpVals(line);

  fi->ExitScope(fi->PC() + 1);
}

/*
   ________________________________
 |           _______________       |
 |          / false? jmp    |      |
 |         /                |      |
 | while exp do block end <-'      |
 |       ^          \              |
 |       |__________/              |
   ________________________________
      \                    / \  //\
       \    |\___/|      /   \//  \\
            /0  0  \__  /    //  | \ \
           /     /  \/_/    //   |  \  \
           @_^_@'/   \/_   //    |   \   \
           //_^_/     \/_ //     |    \    \
        ( //) |        \///      |     \     \
      ( / /) _|_ /   )  //       |      \     _\
    ( // /) '/,_ _ _/  ( ; -.    |    _ _\.-~        .-~~~^-.
  (( / / )) ,-{        _      `-.|.-~-.           .~         `.
 (( // / ))  '/\      /                 ~-. _ .-~      .-~^-.  \
 (( /// ))      `.   {            }                   /      \  \
  (( / ))     .----~-.\        \-'                 .~         \  `. \^-.
             ///.----..>        \             _ -~             `.  ^-`  ^-_
               ///-._ _ _ _ _ _ _}^ - - - - ~                     ~-- ,.-~
                                                                  /.-~
*/
void CGWhileStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::WhileStat> &stat) {
  auto pc_before_exp = fi->PC();

  auto old_regs = fi->used_regs_;
  auto[arg, arg_kink]=ExpToOpArg(fi, stat->exp_, ArgKind::kReg);
  fi->used_regs_ = old_regs;

  auto line = LastLineOf(stat->exp_);
  fi->EmitTest(line, arg, 0);
  auto pc_jmp_to_end = fi->EmitJmp(line, 0, 0);

  fi->EnterScope(true);
  CGBlock(fi, stat->block_);
  fi->CloseOpenUpVals(stat->block_->last_line_);
  fi->EmitJmp(stat->block_->last_line_, 0, pc_before_exp - fi->PC() - 1);
  fi->ExitScope(fi->PC());

  fi->FixSBx(pc_jmp_to_end, fi->PC() - pc_jmp_to_end);
}

/*
 -----------------------------------------------------------------------------
|          _________________       _________________       _____________      |
|         / false? jmp      |     / false? jmp      |     / false? jmp  |     |
|        /                  V    /                  V    /              V     |
| if exp1 then block1 elseif exp2 then block2 elseif true then block3 end <-. |
|                    \                       \                       \      | |
|                     \_______________________\_______________________\_____| |
|                     jmp                     jmp                     jmp     |
|                                                                             |
 -----------------------------------------------------------------------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
*/
void CGIfStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::IfStat> &stat) {
  std::vector<int> pc_jmp_to_ends(stat->exps_.size());
  auto pc_jmp_to_next_exp = -1;

  for (size_t i = 0; i < stat->exps_.size(); i++) {
    auto exp = stat->exps_[i];

    if (pc_jmp_to_next_exp >= 0)
      fi->FixSBx(pc_jmp_to_next_exp, fi->PC() - pc_jmp_to_next_exp);

    auto old_regs = fi->used_regs_;
    auto[arg, arg_kind]=ExpToOpArg(fi, exp, ArgKind::kReg);
    fi->used_regs_ = old_regs;

    auto line = LastLineOf(exp);
    fi->EmitTest(line, arg, 0);
    pc_jmp_to_next_exp = fi->EmitJmp(line, 0, 0);

    auto block = stat->block_[i];
    fi->EnterScope(false);
    CGBlock(fi, block);
    fi->CloseOpenUpVals(block->last_line_);
    fi->ExitScope(fi->PC() + 1);

    if (i < stat->exps_.size() - 1)
      pc_jmp_to_ends[i] = fi->EmitJmp(block->last_line_, 0, 0);
    else
      pc_jmp_to_ends[i] = pc_jmp_to_next_exp;
  }

  for (const auto &pc:pc_jmp_to_ends)
    fi->FixSBx(pc, fi->PC() - pc);
}

void CGForNumStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::ForNumStat> &stat) {
  static const std::string for_index_var = "(for index)";
  static const std::string for_limit_var = "(for limit)";
  static const std::string for_step_var = "(for step)";

  fi->EnterScope(true);

  CGLocalVarDeclStat(fi, std::make_shared<ast::LocalVarDeclStat>(
      0,
      std::vector<std::string>{for_index_var, for_limit_var, for_step_var},
      std::vector<std::shared_ptr<ast::Exp>>{stat->init_exp_, stat->limit_exp_, stat->step_exp_}));
  fi->AddLocVar(stat->var_name_, fi->PC() + 2);

  auto a = fi->used_regs_ - 4;
  auto pc_for_prep = fi->EmitForPrep(stat->line_of_do_, a, 0);
  CGBlock(fi, stat->block_);
  fi->CloseOpenUpVals(stat->block_->last_line_);
  auto pc_for_loop = fi->EmitForLoop(stat->line_of_for_, a, 0);

  fi->FixSBx(pc_for_prep, pc_for_loop - pc_for_prep - 1);
  fi->FixSBx(pc_for_loop, pc_for_prep - pc_for_loop);

  fi->ExitScope(fi->PC());
  fi->FixEndPC(for_index_var, 1);
  fi->FixEndPC(for_limit_var, 1);
  fi->FixEndPC(for_step_var, 1);
}

void CGForInStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::ForInStat> &stat) {
  static std::string for_generator_var = "(for generator)";
  static std::string for_state_var = "(for state)";
  static std::string for_control_var = "(for control)";

  fi->EnterScope(true);

  CGLocalVarDeclStat(fi, std::make_shared<ast::LocalVarDeclStat>(
      0,
      std::vector<std::string>{for_generator_var, for_state_var, for_control_var},
      stat->exp_list_));
  for (const auto &name:stat->name_list_)
    fi->AddLocVar(name, fi->PC() + 2);

  auto pc_jmp_to_tfc = fi->EmitJmp(stat->line_of_do_, 0, 0);
  CGBlock(fi, stat->block_);
  fi->CloseOpenUpVals(stat->block_->last_line_);
  fi->FixSBx(pc_jmp_to_tfc, fi->PC() - pc_jmp_to_tfc);

  auto line = LineOf(stat->exp_list_[0]);
  auto r_generator = fi->SlotOfLocVar(for_generator_var);
  fi->EmitTForCall(line, r_generator, stat->name_list_.size());
  fi->EmitTForLoop(line, r_generator + 2, pc_jmp_to_tfc - fi->PC() - 1);

  fi->ExitScope(fi->PC() - 1);
  fi->FixEndPC(for_generator_var, 2);
  fi->FixEndPC(for_state_var, 2);
  fi->FixEndPC(for_control_var, 2);
}

void CGAssignStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::AssignStat> &stat) {
  auto opt_exps = RemoveTailNils(stat->exp_list_);
  auto exps = opt_exps ? opt_exps.value() : std::vector<std::shared_ptr<ast::Exp>>{};
  auto n_exps = exps.size();
  auto n_vars = stat->var_list_.size();

  std::vector<int32_t> t_regs(n_vars);
  std::vector<int32_t> k_regs(n_vars);
  std::vector<int32_t> v_regs(n_vars);
  auto old_regs = fi->used_regs_;

  for (size_t i = 0; i < stat->var_list_.size(); ++i) {
    const auto &exp = stat->var_list_[i];
    auto ta_exp = std::dynamic_pointer_cast<ast::TableAccessExp>(exp);
    if (exp) {
      t_regs[i] = fi->AllocReg();
      CGExp(fi, ta_exp->prefix_exp_, t_regs[i], 1);
      k_regs[i] = fi->AllocReg();
      CGExp(fi, ta_exp->key_exp_, k_regs[i], 1);
    } else {
      auto name = std::dynamic_pointer_cast<ast::NameExp>(exp)->name_;
      if (fi->SlotOfLocVar(name) < 0 && fi->IndexOfConstant(name) < 0) {
        k_regs[i] = -1;
        if (fi->IndexOfConstant(name) > 0xFF)
          k_regs[i] = fi->AllocReg();
      }
    }
  }

  for (size_t i = 0; i < n_vars; ++i)
    v_regs[i] = fi->used_regs_ + i;

  if (n_exps >= n_vars) {
    for (size_t i = 0; i < exps.size(); ++i) {
      const auto &exp = exps[i];
      auto a = fi->AllocReg();
      if (i >= n_vars && i == n_exps - 1 && IsVarargOrFuncCall(exp))
        CGExp(fi, exp, a, 0);
      else
        CGExp(fi, exp, a, 1);
    }
  } else {
    auto mult_ret = false;
    for (size_t i = 0; i < exps.size(); ++i) {
      auto exp = exps[i];
      auto a = fi->AllocReg();
      if (i == n_exps - 1 && IsVarargOrFuncCall(exp)) {
        mult_ret = true;
        auto n = n_vars - n_exps + 1;
        CGExp(fi, exp, a, n);
        fi->AllocRegs(n - 1);
      } else
        CGExp(fi, exp, a, 1);
    }
    if (!mult_ret) {
      auto n = n_vars - n_exps;
      auto a = fi->AllocRegs(n);
      fi->EmitLoadNil(stat->last_line_, a, n);
    }
  }

  auto last_line = stat->last_line_;
  for (size_t i = 0; i < stat->var_list_.size(); ++i) {
    const auto &exp = stat->var_list_[i];
    auto named_exp = std::dynamic_pointer_cast<ast::NameExp>(exp);
    if (named_exp) {
      auto var_name = named_exp->name_;
      int32_t a{}, b{};

      a = fi->SlotOfLocVar(var_name);
      b = fi->IndexOfUpVal(var_name);

      if (a >= 0)
        fi->EmitMove(last_line, a, v_regs[i]);
      else if (b >= 0)
        fi->EmitSetUpVal(last_line, v_regs[i], b);
      else {
        a = fi->SlotOfLocVar("_ENV");
        if (a >= 0) {
          if (k_regs[i < 0]) {
            b = 0x100 + fi->IndexOfConstant(var_name);
            fi->EmitSetTable(last_line, a, b, v_regs[i]);
          } else
            fi->EmitSetTable(last_line, a, k_regs[i], v_regs[i]);
        } else {
          a = fi->IndexOfUpVal("_ENV");
          if (k_regs[i] < 0) {
            b = 0x100 + fi->IndexOfConstant(var_name);
            fi->EmitSetTabUp(last_line, a, b, v_regs[i]);
          } else
            fi->EmitSetTabUp(last_line, a, k_regs[i], v_regs[i]);
        }
      }
    } else
      fi->EmitSetTabUp(last_line, t_regs[i], k_regs[i], v_regs[i]);
  }

  // TODO
  fi->used_regs_ = old_regs;
}

void CGLocalVarDeclStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::LocalVarDeclStat> &stat) {
  auto opt_exps = RemoveTailNils(stat->exp_list_);
  auto exps = opt_exps ? opt_exps.value() : std::vector<std::shared_ptr<ast::Exp>>{};
  auto n_exps = exps.size();
  auto n_names = stat->name_list_.size();

  auto old_regs = fi->used_regs_;
  if (n_exps == n_names) {
    for (const auto &exp:exps) {
      auto a = fi->AllocReg();
      CGExp(fi, exp, a, 1);
    }
  } else if (n_exps > n_names) {
    for (size_t i = 0; i < exps.size(); ++i) {
      const auto &exp = exps[i];
      auto a = fi->AllocReg();
      if (i == n_exps - 1 && IsVarargOrFuncCall(exp))
        CGExp(fi, exp, a, 0);
      else
        CGExp(fi, exp, a, 1);
    }
  } else {
    auto mult_ret = false;
    for (size_t i = 0; i < exps.size(); ++i) {
      const auto &exp = exps[i];
      auto a = fi->AllocReg();
      if (i == n_exps - 1 && IsVarargOrFuncCall(exp)) {
        mult_ret = true;
        auto n = n_names - n_exps + 1;
        CGExp(fi, exp, a, n);
        fi->AllocRegs(n - 1);
      } else
        CGExp(fi, exp, a, 1);
    }
    if (!mult_ret) {
      auto n = n_names - n_exps;
      auto a = fi->AllocRegs(n);
      fi->EmitLoadNil(stat->last_line_, a, n);
    }
  }

  fi->used_regs_ = old_regs;
  auto start_pc = fi->PC() + 1;
  for (const auto &name:stat->name_list_)
    fi->AddLocVar(name, start_pc);
}

void CGLabelStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::LabelStat> &stat) {
  THROW_NOT_IMPLEMENTED;
}

void CGGotoStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::GotoStat> &stat) {
  THROW_NOT_IMPLEMENTED;
}
}