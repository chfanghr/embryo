//
// Created by 方泓睿 on 2020/4/8.
//

#include <embryo/utils/inspection.h>
#include <embryo/state/lua_state.h>

namespace embryo::state {

int64_t LuaState::GetTop() const {
  return stack_->top_;
}

int64_t LuaState::AbsIndex(int64_t idx) const {
  return stack_->AbsIndex(idx);
}

bool LuaState::CheckStack(size_t n) {
  stack_->Check(n);
  return true; // never failed
}

void LuaState::Pop(size_t n) {
  for (size_t i = 0; i < n; i++)
    stack_->Pop();
}

void LuaState::Copy(int64_t from, int64_t to) {
  stack_->Set(to, stack_->Get(from));
}

void LuaState::PushValue(int64_t idx) {
  stack_->Push(stack_->Get(idx));
}

void LuaState::Replace(int64_t idx) {
  stack_->Set(idx, stack_->Pop());
}

void LuaState::Insert(int64_t idx) {
  Rotate(idx, 1);
}

void LuaState::Remove(int64_t idx) {
  Rotate(idx, -1);
  Pop(1);
}

void LuaState::Rotate(int64_t idx, size_t n) {
  auto t = stack_->top_ - 1;
  auto p = stack_->AbsIndex(idx) - 1;
  int64_t m{};

  if (n >= 0)
    m = t - n;
  else
    m = p - n - 1;

  stack_->Reverse(p, m);
  stack_->Reverse(m + 1, t);
  stack_->Reverse(p, t);
}

void LuaState::SetTop(int64_t idx) {
  auto new_top = AbsIndex(idx);
  if (new_top < 0)
    Error(MSG_WITH_FUNC_NAME("internal stack underflow"));

  auto n = stack_->top_ - new_top;

  if (n > 0)
    Pop(n);
  else if (n < 0)
    for (size_t i = 0; i < -n; i++)
      stack_->Push(nullptr);
}

void LuaState::XMove(const std::shared_ptr<api::LuaState> &to, size_t n) {
  auto vals = stack_->PopN(n);
  std::dynamic_pointer_cast<LuaState>(to)->stack_->PushN(vals, n);
}
}