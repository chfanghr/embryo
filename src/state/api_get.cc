//
// Created by 方泓睿 on 2020/4/10.
//

#include <embryo/state/lua_value.h>
#include <embryo/state/lua_table.h>
#include <embryo/state/lua_state.h>

namespace embryo::state {
void LuaState::CreateTable(int64_t n_arr, int64_t n_rec) {
  auto table = std::make_shared<LuaTable>(n_arr, n_rec);
  stack_->Push(std::dynamic_pointer_cast<LuaAnyType>(table));
}

void LuaState::NewTable() {
  CreateTable(0, 0);
}

api::LuaValueType LuaState::GetTable(int64_t idx) {
  auto t = stack_->Get(idx);
  auto k = stack_->Pop();
  return GetTable(t, k, false);
}

api::LuaValueType LuaState::GetTable(const embryo::state::LuaValue &t, const embryo::state::LuaValue &k, bool raw) {
  auto[table, ok] = TryCast<LuaTablePtr>(t);
  if (ok) {
    auto v = table->Get(k);
    if (raw || v || !table->HasMetaField("__index")) {
      stack_->Push(v);
      return TypeOf(v);
    }
  }
  if (!raw) {
    auto mf = state::GetMetaField(t, "__index", shared_from_this());
    if (mf) {
      switch (TypeOf(mf)) {
        case api::LuaValueType::kTable:return GetTable(mf, k, false);
        case api::LuaValueType::kFunction: {
          stack_->Push(mf);
          stack_->Push(t);
          stack_->Push(k);
          Call(2, 1);
          auto v = stack_->Get(-1);
          return TypeOf(v);
        }
        default:;
      }
    }
  }
  Error(MSG_WITH_FUNC_NAME("index is not supported on this type"));
}

api::LuaValueType LuaState::GetField(
    int64_t idx, std::string k) {
  auto t = stack_->Get(idx);
  return GetTable(t, k, false);
}

api::LuaValueType LuaState::GetI(int64_t idx, int64_t i) {
  auto table = stack_->Get(idx);
  return GetTable(table, i, false);
}

bool LuaState::GetMetatable(int64_t idx) {
  auto val = stack_->Get(idx);
  auto mt = state::GetMetaTable(val, shared_from_this());
  if (mt)
    stack_->Push(mt);
  return bool(mt);
}

api::LuaValueType LuaState::RawGet(int64_t idx) {
  auto t = stack_->Get(idx);
  auto k = stack_->Pop();
  return GetTable(t, k, true);
}

api::LuaValueType LuaState::RawGetI(int64_t idx, int64_t i) {
  auto t = stack_->Get(idx);
  return GetTable(t, i, true);
}
}