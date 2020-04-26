//
// Created by 方泓睿 on 2020/4/10.
//

#include <embryo/state/lua_value.h>
#include <embryo/state/lua_state.h>
#include <embryo/state/lua_table.h>
#include <embryo/state/lua_value.h>

namespace embryo::state {
void LuaState::SetTable(int64_t idx) {
  auto t = stack_->Get(idx);
  auto v = stack_->Pop();
  auto k = stack_->Pop();

  SetTable(t, k, v, false);
}

void LuaState::SetTable(const embryo::state::LuaValue &t,
                        const embryo::state::LuaValue &k,
                        const embryo::state::LuaValue &v,
                        bool raw) {
  auto[table, ok] = TryCast<LuaTablePtr>(t);
  if (ok) {
    if (raw || table->Get(k) || !table->HasMetaField("__newindex")) {
      table->Put(k, v);
      return;
    }
  }
  if (!raw) {
    auto mf = state::GetMetaField(t, "__index", shared_from_this());
    if (mf) {
      switch (TypeOf(mf)) {
        case api::LuaValueType::kTable:SetTable(mf, k, v, false);
          return;
        case api::LuaValueType::kFunction: {
          stack_->Push(mf);
          stack_->Push(t);
          stack_->Push(k);
          stack_->Push(v);
          Call(3, 0);
          return;
        }
        default:;
      }
    }
  }
  Error(MSG_WITH_FUNC_NAME("index is not supported on this type"));
}

void LuaState::SetField(int64_t idx, std::string k) {
  auto t = stack_->Get(idx);
  auto v = stack_->Pop();
  SetTable(t, k, v, false);
}

void LuaState::SetI(int64_t idx, int64_t i) {
  auto t = stack_->Get(idx);
  auto v = stack_->Pop();
  SetTable(t, i, v, false);
}

void LuaState::RawSet(int64_t idx) {
  auto t = stack_->Get(idx);
  auto v = stack_->Pop();
  auto k = stack_->Pop();
  SetTable(t, k, v, true);
}

void LuaState::RawSetI(int64_t idx, int64_t i) {
  auto t = stack_->Get(idx);
  auto v = stack_->Pop();
  SetTable(t, i, v, true);
}

void LuaState::SetMetatable(int64_t idx) {
  auto val = stack_->Get(idx);
  auto mt_val = stack_->Pop();

  if (!mt_val)
    state::SetMetaTable(val, nullptr, shared_from_this());
  else {
    auto[mt, ok]=TryCast<LuaTablePtr>(mt_val);
    if (ok) {
      state::SetMetaTable(val, mt, shared_from_this());
      return;
    }
  }
  Error(MSG_WITH_FUNC_NAME("table expected")); // TODO
}
}