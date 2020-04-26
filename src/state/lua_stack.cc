//
// Created by 方泓睿 on 2020/4/8.
//

#include <stdexcept>
#include <utility>

#include <fmt/format.h>

#include <embryo/utils/inspection.h>
#include <embryo/state/lua_stack.h>
#include <embryo/state/lua_state.h>
#include <embryo/state/lua_table.h>

namespace embryo::state {
LuaStack::LuaStack(size_t init_size, LuaState *lua_state)
    : slots_(init_size), top_(0), state_(lua_state) {}

void LuaStack::Check(size_t n) {
  auto free = slots_.size() - top_;
  if (free < n)
    slots_.resize(slots_.size() + n);
}

void LuaStack::Push(LuaValue val) {
  if (top_ == slots_.size())
    Error(MSG_WITH_FUNC_NAME("overflow"));

  slots_[top_] = std::move(val);
  ++top_;
}

LuaValue LuaStack::Pop() {
  if (top_ < 1)
    Error(MSG_WITH_FUNC_NAME("underflow"));
  --top_;
  auto val = slots_[top_];
  slots_[top_] = {};
  return val;
}

int64_t LuaStack::AbsIndex(int64_t idx) const noexcept {
  if (idx >= 0 || idx <= kLuaRegistryIndex)
    return idx;
  return idx + top_ + 1;
}

bool LuaStack::IsValidIndex(int64_t idx) const {
  if (idx < kLuaRegistryIndex) {
    auto uv_idx = kLuaRegistryIndex - idx - 1;
    auto c = closure_;
    return c && uv_idx < c->up_values.size();
  }
  if (idx == kLuaRegistryIndex)
    return idx;
  auto abs_idx = AbsIndex(idx);
  return abs_idx > 0 && abs_idx <= top_;
}

LuaValue LuaStack::Get(int64_t idx) const {
  if (idx < kLuaRegistryIndex) {
    auto uv_idx = kLuaRegistryIndex - idx - 1;
    auto c = closure_;
    if (!c || uv_idx >= c->up_values.size())
      return {};
    return c->up_values[uv_idx]->val_;
  }
  if (idx == kLuaRegistryIndex)
    return std::dynamic_pointer_cast<LuaAnyType>(state_->registry_);
  ValidateIndex(idx);
  return slots_[AbsIndex(idx) - 1];
}

void *LuaStack::GetPointer(int64_t idx) {
  if (idx < kLuaRegistryIndex) {
    auto uv_idx = kLuaRegistryIndex - idx - 1;
    auto c = closure_;
    if (!c || uv_idx >= c->up_values.size())
      return {};
    return (void *) &c->up_values[uv_idx]->val_;
  }
  if (idx == kLuaRegistryIndex)
    return (void *) std::dynamic_pointer_cast<LuaAnyType>(state_->registry_).get();
  ValidateIndex(idx);
  return (void *) &slots_[AbsIndex(idx) - 1];
}

void LuaStack::Set(int64_t idx, LuaValue value) {
  if (idx < kLuaRegistryIndex) {
    auto uv_idx = kLuaRegistryIndex - idx - 1;
    auto c = closure_;
    if (c && uv_idx < c->up_values.size())
      c->up_values[uv_idx]->val_ = value;
    return;
  }
  if (idx == kLuaRegistryIndex) {
    state_->registry_ = Cast<LuaTablePtr>(value);
    return;
  }
  ValidateIndex(idx);
  slots_[AbsIndex(idx) - 1] = std::move(value);
}

void LuaStack::ValidateIndex(int64_t idx) const {
  if (!IsValidIndex(idx))
    Error(MSG_WITH_FUNC_NAME("invalid index"));
}

void LuaStack::Error(const std::string &msg) const {
  throw std::runtime_error(fmt::format("{}: {}", THIS_TYPE, msg));
}

void LuaStack::Reverse(int64_t from, int64_t to) {
  while (from < to) {
    std::tie(slots_[from], slots_[to]) = std::tuple(slots_[to], slots_[from]);
    ++from;
    --to;
  }
}

std::vector<LuaValue> LuaStack::PopN(size_t n) {
  std::vector<LuaValue> res(n);
  for (int64_t i = n - 1; i >= 0; i--)
    res[i] = Pop();
  return res;
}

void LuaStack::PushN(const std::vector<LuaValue> &vals, int64_t n) {
  auto n_vals = vals.size();
  if (n < 0) n = n_vals;
  for (int64_t i = 0; i < n; i++) {
    if (i < n_vals)
      Push(vals[i]);
    else
      Push(nullptr);
  }
}

}