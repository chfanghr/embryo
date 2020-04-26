//
// Created by 方泓睿 on 2020/4/8.
//

#include <embryo/state/lua_state.h>
#include <embryo/state/lua_table.h>

namespace embryo::state {
void LuaState::PushNil() { stack_->Push(nullptr); }

void LuaState::PushNumber(LuaNumber number) { stack_->Push(number); }

void LuaState::PushBoolean(bool b) { stack_->Push(b); }

void LuaState::PushInteger(LuaInteger integer) { stack_->Push(integer); }

void LuaState::PushString(std::string string) { stack_->Push(string); }

void LuaState::PushNativeFunction(NativeFunction f) {
  stack_->Push(
      std::dynamic_pointer_cast<LuaAnyType>(
          std::make_shared<bin_chunk::Closure>(std::move(f), 0)));
}

bool LuaState::IsNativeFunction(int32_t idx) {
  return ContainsType<NativeFunction>(stack_->Get(idx));
}

NativeFunction LuaState::ToNativeFunction(int32_t idx) {
  return Cast<NativeFunction>(stack_->Get(idx));
}

void LuaState::PushGlobalTable() {
  auto global = registry_->Get(kLuaRIdxGlobals);
  stack_->Push(global);
}

api::LuaValueType LuaState::GetGlobal(const std::string &name) {
  auto global = registry_->Get(kLuaRIdxGlobals);
  return GetTable(global, name, false);
}

void LuaState::SetGlobal(const std::string &name) {
  auto global = registry_->Get(kLuaRIdxGlobals);
  auto value = stack_->Pop();
  SetTable(global, name, value, false);
}

void LuaState::Register(const std::string &name, NativeFunction f) {
  RegisterInternal(name, f);
}

void LuaState::PushNativeClosure(NativeFunction f, int32_t n) {
  auto closure = std::make_shared<bin_chunk::Closure>(f, n);
  for (int64_t i = n; i > 0; i--) {
    auto val = stack_->Pop();
    closure->up_values[n - 1] = std::make_shared<UpValue>(val);
  }
  stack_->Push(closure);
}

void LuaState::RegisterInternal(const std::string &name, state::NativeFunction f) {
  PushNativeFunction(std::move(f));
  SetGlobal(name);
}

bool LuaState::PushThread() {
  stack_->Push(std::dynamic_pointer_cast<LuaAnyType>(shared_from_this()));
  return IsMainThread();
}
}