//
// Created by 方泓睿 on 2020/4/5.
//

#include <utility>

#include <embryo/utils/formatters.h>
#include <embryo/utils/inspection.h>
#include <embryo/compiler/ast/block.h>
#include <embryo/compiler/ast/common.h>
#include <embryo/compiler/ast/stat.h>
#include <embryo/compiler/ast/exp.h>

namespace embryo::ast {
std::string Block::String() const {
  return INSPECT_THIS
      .Add(KV_AS_STRING(chunk_name_))
      .Add(KV(last_line_))
      .Add(KV(stats_))
      .Add(KV(ret_exps_))
      .Result();
}

std::string BreakStat::String() const {
  return INSPECT_THIS.Add(KV(line_)).Result();
}

std::string EmptyStat::String() const {
  return INSPECT_THIS.Result();
}

std::string LabelStat::String() const {
  return INSPECT_THIS.Add(KV_AS_STRING(name_)).Result();
}

std::string GotoStat::String() const {
  return INSPECT_THIS.Add(KV_AS_STRING(name_)).Result();
}

std::string DoStat::String() const {
  return INSPECT_THIS.Add(KV(block_)).Result();
}

std::string WhileStat::String() const {
  return INSPECT_THIS.Add(KV(exp_)).Add(KV(block_)).Result();
}

std::string RepeatStat::String() const {
  return INSPECT_THIS.Add(KV(exp_)).Add(KV(block_)).Result();
}

std::string IfStat::String() const {
  return INSPECT_THIS.Add(KV(exps_)).Add(KV(block_)).Result();
}

std::string ForNumStat::String() const {
  return INSPECT_THIS
      .Add(KV(line_of_for_))
      .Add(KV(line_of_do_))
      .Add(KV_AS_STRING(var_name_))
      .Add(KV(init_exp_))
      .Add(KV(limit_exp_))
      .Add(KV(block_))
      .Result();
}

std::string ForInStat::String() const {
  return INSPECT_THIS
      .Add(KV(line_of_do_))
      .Add(KV_AS_STRINGS(name_list_))
      .Add(KV(exp_list_))
      .Add(KV(block_))
      .Result();
}

std::string LocalVarDeclStat::String() const {
  return INSPECT_THIS
      .Add(KV(last_line_))
      .Add(KV_AS_STRINGS(name_list_))
      .Add(KV(exp_list_))
      .Result();
}

std::string AssignStat::String() const {
  return INSPECT_THIS
      .Add(KV(last_line_))
      .Add(KV(var_list_))
      .Add(KV(exp_list_))
      .Result();
}

std::string LocalFuncDefStat::String() const {
  return INSPECT_THIS
      .Add(KV_AS_STRING(name_))
      .Add(KV(func_def_exp_))
      .Result();
}

std::string NilExp::String() const {
  return INSPECT_THIS.Add(KV(line_)).Result();
}

std::string TrueExp::String() const {
  return INSPECT_THIS.Add(KV(line_)).Result();
}

std::string FalseExp::String() const {
  return INSPECT_THIS.Add(KV(line_)).Result();
}

std::string VarArgExp::String() const {
  return INSPECT_THIS.Add(KV(line_)).Result();
}

std::string IntegerExp::String() const {
  return INSPECT_THIS.Add(KV(line_)).Add(KV(val_)).Result();
}

std::string FloatExp::String() const {
  return INSPECT_THIS.Add(KV(line_)).Add(KV(val_)).Result();
}

std::string StringExp::String() const {
  return INSPECT_THIS.Add(KV(line_)).Add(KV_AS_STRING(str_)).Result();
}

std::string NameExp::String() const {
  return INSPECT_THIS.Add(KV(line_)).Add(KV_AS_STRING(name_)).Result();
}

std::string UnOpExp::String() const {
  return INSPECT_THIS
      .Add(KV(line_))
      .Add("op_", AS_STRING(lexer::GetTokenName(op_)))
      .Add(KV(exp_))
      .Result();
}

std::string BinOpExp::String() const {
  return INSPECT_THIS
      .Add(KV(line_))
      .Add("op_", AS_STRING(lexer::GetTokenName(op_)))
      .Add(KV(exp_1_))
      .Add(KV(exp_2_))
      .Result();
}

std::string ConcatExp::String() const {
  return INSPECT_THIS
      .Add(KV(line_))
      .Add(KV(exps_))
      .Result();
}

std::string TableConstructorExp::String() const {
  return INSPECT_THIS
      .Add(KV(line_))
      .Add(KV(last_line_))
      .Add(KV(key_exps_))
      .Add(KV(val_exps_))
      .Result();
}

std::string FuncDefExp::String() const {
  return INSPECT_THIS
      .Add(KV(line_))
      .Add(KV(last_line_))
      .Add(KV_AS_STRINGS(par_list_))
      .Add(KV(is_var_arg_))
      .Add(KV(block_))
      .Result();
}

std::string ParensExp::String() const {
  return INSPECT_THIS.Add(KV(exp_)).Result();
}

std::string TableAccessExp::String() const {
  return INSPECT_THIS
      .Add(KV(last_line_))
      .Add(KV(prefix_exp_))
      .Add(KV(key_exp_))
      .Result();
}

std::string FuncCallExp::String() const {
  return INSPECT_THIS
      .Add(KV(line_))
      .Add(KV(last_line_))
      .Add(KV(prefix_exp_))
      .Add(KV(name_exp_))
      .Add(KV(args_))
      .Result();
}
}