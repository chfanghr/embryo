//
// Created by 方泓睿 on 2020/4/9.
//

#include <embryo/bin_chunk/closure.h>
#include <embryo/state/lua_state.h>

namespace embryo::state {
int32_t LuaState::PC() const {
  return stack_->pc_;
}

void LuaState::AddPC(int32_t n) {
  stack_->pc_ += n;
}

uint32_t LuaState::Fetch() {
  auto code = stack_->closure_->proto_->code[stack_->pc_];
  ++stack_->pc_;
  return code;
}

void LuaState::GetConst(int32_t idx) {
  stack_->Push(ToLuaValue(stack_->closure_->proto_->constants[idx]));
}

void LuaState::GetRK(int32_t rk) {
  if (rk > 0xFF)
    GetConst(rk & 0xFF); // NOLINT(hicpp-signed-bitwise)
  else
    PushValue(rk + 1);
}

int32_t LuaState::RegisterCount() const {
  return stack_->closure_->proto_->max_stack_size;
}

void LuaState::LoadVarArg(int32_t n) {
  if (n < 0)
    n = stack_->var_args_.size();
  stack_->Check(n);
  stack_->PushN(stack_->var_args_, n);
}

void LuaState::LoadProto(int32_t idx) {
  auto stack = stack_;
  auto sub_proto = stack_->closure_->proto_->protos[idx];
  auto closure = std::make_shared<LuaClosure>(sub_proto);
  stack->Push(std::dynamic_pointer_cast<LuaAnyType>(closure));
  for (size_t i = 0; i < sub_proto->up_values.size(); i++) {
    auto uv_info = sub_proto->up_values[i];
    int64_t uv_idx = uv_info.idx;

    if (uv_info.in_stack == 1) {
      auto iter = stack->open_uvs.find(uv_idx);
      if (iter != stack->open_uvs.end())
        closure->up_values[i] = stack->open_uvs[uv_idx];
      else {
        closure->up_values[i] = std::make_shared<UpValue>(stack_->slots_[uv_idx]);
        stack_->open_uvs[uv_idx] = closure->up_values[i];
      }
    } else
      closure->up_values[i] = stack->closure_->up_values[uv_idx];
  }
}

void LuaState::CloseUpValues(int32_t a) {
  auto open_uvs = stack_->open_uvs;
  for (auto[k, v]: open_uvs) {
    if (k >= a - 1) {
      stack_->open_uvs.erase(k);
    }
  }
}
}