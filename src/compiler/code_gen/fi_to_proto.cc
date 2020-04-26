//
// Created by 方泓睿 on 2020/4/16.
//

#include <embryo/compiler/code_gen/fi_to_proto.h>

namespace embryo::code_gen {
namespace {
std::vector<std::shared_ptr<bin_chunk::Prototype>> ToProtos(const std::vector<std::shared_ptr<FuncInfo>> &fis) {
  std::vector<std::shared_ptr<bin_chunk::Prototype>> protos(fis.size());
  for (size_t i = 0; i < fis.size(); i++)
    protos[i] = ToProto(fis[i]);
  return protos;
}

std::vector<LuaConstant> GetConstants(const std::shared_ptr<FuncInfo> &fi) {
  std::vector<LuaConstant> consts(fi->constants_.size());
  for (const auto &[k, idx]:fi->constants_)
    consts[idx] = k;
  return consts;
}

std::vector<bin_chunk::LocVar> GetLocVars(const std::shared_ptr<FuncInfo> &fi) {
  std::vector<bin_chunk::LocVar> loc_vars(fi->loc_vars_.size());
  for (size_t i = 0; i < fi->loc_vars_.size(); i++) {
    const auto &loc_var = fi->loc_vars_[i];
    loc_vars[i] = bin_chunk::LocVar{
        loc_var->name,
        static_cast<uint32_t>(loc_var->start_pc_),
        static_cast<uint32_t>(loc_var->end_pc_)
    };
  }
  return loc_vars;
}

std::vector<bin_chunk::UpValue> GetUpValues(const std::shared_ptr<FuncInfo> &fi) {
  std::vector<bin_chunk::UpValue> up_vals(fi->up_values_.size());
  for (const auto &[k, uv]:fi->up_values_) {
    if (uv.loc_var_slot >= 0)
      up_vals[uv.index] = bin_chunk::UpValue(1, uv.loc_var_slot);
    else
      up_vals[uv.index] = bin_chunk::UpValue(0, uv.up_val_index);
  }
  return up_vals;
}

std::vector<std::string> GetUpValueNames(const std::shared_ptr<FuncInfo> &fi) {
  std::vector<std::string> names(fi->up_values_.size());
  for (const auto &[k, uv]:fi->up_values_)
    names[uv.index] = k;
  return names;
}
}

std::shared_ptr<bin_chunk::Prototype> ToProto(const std::shared_ptr<FuncInfo> &fi) {
  return std::make_shared<bin_chunk::Prototype>(
      "",
      fi->line_,
      fi->line_ == 0 ? 0 : fi->last_line_,
      fi->num_params_,
      fi->is_var_arg ? 1 : 0, // TODO
      fi->max_regs_ < 2 ? 2 : fi->max_regs_, // TODO
      fi->insts_,
      GetConstants(fi),
      GetUpValues(fi),
      ToProtos(fi->sub_funcs_),
      fi->line_nums_,
      GetLocVars(fi),
      GetUpValueNames(fi));
}
}