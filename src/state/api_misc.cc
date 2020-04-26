//
// Created by 方泓睿 on 2020/4/8.
//

#include <embryo/utils/numbers.h>
#include <embryo/utils/inspection.h>
#include <embryo/state/lua_state.h>
#include <embryo/state/lua_table.h>

namespace embryo::state {
void LuaState::Len(int64_t idx) {
  {
    auto[val, ok] =TryCast<std::string>(stack_->Get(idx));
    if (ok) {
      stack_->Push(int64_t(val.size()));
      return;
    }
  }
  {
    auto val = stack_->Get(idx);
    auto res = CallMetaMethod(val, val, "__len", shared_from_this());
    if (res)
      stack_->Push(res);
  }
  {
    auto[ptr, ok] =TryCast<LuaTablePtr>(stack_->Get(idx));
    if (ok) {
      stack_->Push(int64_t(ptr->Len()));
      return;
    }
  }
  Error(
      MSG_WITH_FUNC_NAME("len error: unsupported type"));
}

size_t LuaState::RawLen(int64_t idx) {
  {
    auto[val, ok] =TryCast<std::string>(stack_->Get(idx));
    if (ok)
      return val.size();
  }
  {
    auto[ptr, ok] =TryCast<LuaTablePtr>(stack_->Get(idx));
    if (ok)
      return ptr->Len();
  }
  return 0;
}

void LuaState::Concat(size_t n) {
  if (n == 0)
    stack_->Push("");
  else if (n >= 2) {
    for (size_t i = 1; i < n; i++) {
      if (IsString(-1) && IsString(-2)) {
        auto s_2 = ToString(-1);
        auto s_1 = ToString(-2);
        stack_->Pop();
        stack_->Pop();
        stack_->Push(s_1 + s_2);
        continue;
      }

      auto a = stack_->Pop();
      auto b = stack_->Pop();
      auto res = CallMetaMethod(a, b, "__concat", shared_from_this());
      if (res) {
        stack_->Push(res);
        continue;
      }
      Error(MSG_WITH_FUNC_NAME("concat error: unsupported type"));
    }
  }
}

bool LuaState::Next(int64_t idx) {
  auto val = stack_->Get(idx);
  auto[t, ok] = TryCast<LuaTablePtr>(val);
  if (ok) {
    auto key = stack_->Pop();
    auto next_key = t->NextKey(key);
    if (next_key) {
      stack_->Push(next_key);
      stack_->Push(t->Get(next_key));
      return true;
    }
    return false;
  }

  Error(MSG_WITH_FUNC_NAME("table expected"));
}

int32_t LuaState::Error() {
  auto err = stack_->Pop();
  throw std::runtime_error(fmt::format("Lua Error: {}", Stringify(err)));
}

bool LuaState::StringToNumber(const std::string &s) {
  {
    auto[n, ok]=utils::ParseInt(s);
    if (ok) {
      PushInteger(n);
      return true;
    }
  }
  {
    auto[n, ok]=utils::ParseFloat(s);
    if (ok) {
      PushNumber(n);
      return true;
    }
  }
  return false;
}
}