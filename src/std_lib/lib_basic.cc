//
// Created by 方泓睿 on 2020/4/19.
//

#include <stdexcept>

#include <fmt/format.h>

#include <embryo/utils/strings.h>
#include <embryo/utils/bytes.h>
#include <embryo/utils/inspection.h>
#include <embryo/std_lib/lib_basic.h>
#include <embryo/version.h>

namespace embryo::std_lib {
api::FuncReg kBaseLib{ // NOLINT(cert-err58-cpp)
    {"print", base::Print},
    {"assert", base::Assert},
    {"error", base::Error},
    {"select", base::Select},
    {"ipairs", base::IPairs},
    {"pairs", base::Pairs},
    {"next", base::Next},
    {"getmetatable", base::GetMetatable},
    {"setmetatable", base::SetMetatable},
    {"pcall", base::PCall},
    {"load", base::Load},
    {"loadfile", base::LoadFile},
    {"dofile", base::DoFile},
    {"xpcall", base::XPCall},
    {"rawequal", base::RawEqual},
    {"rawlen", base::RawLen},
    {"rawget", base::RawGet},
    {"rawset", base::RawSet},
    {"type", base::Type},
    {"tostring", base::ToString},
    {"tonumber", base::ToNumber},
    {"_G", nullptr},
    {"__VERSION", nullptr} // use EMBRYO_VERSION
};

int32_t OpenBaseLib(const std::shared_ptr<api::LuaState> &state) {
  state->PushGlobalTable();
  state->SetFuncs(kBaseLib, 0);

  state->PushValue(-1);
  state->SetField(-2, "_G");
  state->PushString(EMBRYO_VERSION);
  state->SetField(-2, "_VERSION");
  return 1;
}

namespace {
int32_t IPairsAux(const std::shared_ptr<api::LuaState> &state) {
  auto i = state->ToInteger(2) + 1;
  state->PushInteger(i);

  auto flag = state->GetI(1, i);
  if (flag == api::LuaValueType::kNil
      || flag == api::LuaValueType::kNone)
    return 1;
  return 2;
}

int32_t LoadAux(const std::shared_ptr<api::LuaState> &state, api::Status status, int32_t env_idx) {
  if (status == api::Status::kOk) {
    if (env_idx != 0)
      THROW_NOT_IMPLEMENTED; // TODO
    return 1;
  } else {
    state->PushNil();
    state->Insert(-2);
    return 2;
  }
}
}

namespace base {
int32_t Print(const std::shared_ptr<api::LuaState> &state) {
  auto n_args = state->GetTop();
  for (size_t i = 1; i <= n_args; i++) {
    if (state->IsNil(i))
      fmt::print("<nil>");
    else if (state->IsNone(i))
      fmt::print("<none>");
    else if (state->IsString(i))
      fmt::print(state->ToString(i));
    else if (state->IsBoolean(i))
      fmt::print("{}", state->ToBoolean(i));
    if (i < n_args)
      fmt::print("\t");
  }
  fmt::print("\n");
  return 0;
}

int32_t Assert(const std::shared_ptr<api::LuaState> &state) {
  if (state->ToBoolean(1))
    return state->GetTop();
  state->CheckAny(1);
  state->Remove(1);
  state->PushString("assertion failed");
  state->SetTop(1);
  return Error(state);
}

int32_t Error(const std::shared_ptr<api::LuaState> &state) {
  state->Error();
  throw std::runtime_error("unreachable code");
}

int32_t Select(const std::shared_ptr<api::LuaState> &state) {
  int64_t n = state->GetTop();
  if (state->Type(1) == api::LuaValueType::kString && state->CheckString(1) == "#") {
    state->PushInteger(n - 1);
    return 1;
  }

  auto i = state->CheckInteger(1);
  if (i < 0)
    i = n + i;
  else if (i > n)
    i = n;
  state->ArgCheck(1 <= i, 1, "index out of range");
  return n - i;
}

int32_t IPairs(const std::shared_ptr<api::LuaState> &state) {
  state->PushNativeFunction(IPairsAux);
  state->PushValue(1);
  state->PushInteger(0);
  return 3;
}

int32_t Pairs(const std::shared_ptr<api::LuaState> &state) {
  state->PushNativeFunction(Next);
  state->PushValue(1);
  state->PushNil();
  return 3;
}

int32_t Next(const std::shared_ptr<api::LuaState> &state) {
  state->SetTop(2);
  if (state->Next(1))
    return 2;
  state->PushNil();
  return 1;
}

int32_t Load(const std::shared_ptr<api::LuaState> &state) {
  api::Status status{};
  auto[chunk, is_str]=state->ToStringX(1);
  auto mode = state->OptString(3, "bt");
  auto env = 0;
  if (!state->IsNone(4))
    env = 4;
  if (is_str) {
    auto chunk_name = state->OptString(2, chunk);
    status = state->Load(utils::Convert(chunk), chunk_name, mode);
  } else
    throw std::runtime_error("loading from a reader function"); // TODO
  return LoadAux(state, status, env);
}

int32_t LoadFile(const std::shared_ptr<api::LuaState> &state) {
  auto f_name = state->OptString(1, "");
  auto mode = state->OptString(1, "");
  auto env = 0;
  if (!state->IsNone(3))
    env = 3;
  auto status = state->LoadFileX(f_name, mode);
  return LoadAux(state, status, env);
}

int32_t DoFile(const std::shared_ptr<api::LuaState> &state) {
  auto f_name = state->OptString(1, "bt");
  state->SetTop(1);
  if (state->LoadFile(f_name) != api::Status::kOk)
    return state->Error();
  state->Call(0, api::kMultert);
  return state->GetTop() - 1;
}

int32_t PCall(const std::shared_ptr<api::LuaState> &state) {
  auto n_args = state->GetTop() - 1;
  auto status = state->PCall(n_args, -1, 0);
  state->PushBoolean(status == api::Status::kOk);
  state->Insert(1);
  return state->GetTop();
}

int32_t XPCall(const std::shared_ptr<api::LuaState> &) {
  THROW_NOT_IMPLEMENTED;
}

int32_t GetMetatable(const std::shared_ptr<api::LuaState> &state) {
  if (!state->GetMetatable(1))
    state->PushNil();
  return 1;
}

int32_t SetMetatable(const std::shared_ptr<api::LuaState> &state) {
  state->SetMetatable(1);
  return 1;
}

int32_t RawEqual(const std::shared_ptr<api::LuaState> &state) {
  state->CheckAny(1);
  state->CheckAny(2);
  state->PushBoolean(state->RawEqual(1, 2));
  return 1;
}

int32_t RawLen(const std::shared_ptr<api::LuaState> &state) {
  auto t = state->Type(1);
  state->ArgCheck(t == api::LuaValueType::kString ||
      t == api::LuaValueType::kTable, 1, "table or string expected");
  state->PushInteger(state->RawLen(1));
  return 1;
}

int32_t RawGet(const std::shared_ptr<api::LuaState> &state) {
  state->CheckType(1, api::LuaValueType::kTable);
  state->CheckAny(2);
  state->SetTop(2);
  state->RawGet(1);
  return 1;
}

int32_t RawSet(const std::shared_ptr<api::LuaState> &state) {
  state->CheckType(1, api::LuaValueType::kTable);
  state->CheckAny(2);
  state->CheckAny(3);
  state->SetTop(3);
  state->RawSet(1);
  return 1;
}

int32_t Type(const std::shared_ptr<api::LuaState> &state) {
  auto t = state->Type(1);
  state->ArgCheck(t != api::LuaValueType::kNone, 1, "value expected");
  state->PushString(state->TypeName(t));
  return 1;
}

int32_t ToString(const std::shared_ptr<api::LuaState> &state) {
  state->CheckAny(1);
  state->ToString2(1);
  return 1;
}

int32_t ToNumber(const std::shared_ptr<api::LuaState> &state) {
  if (state->IsNoneOrNil(2)) {
    state->CheckAny(1);
    if (state->Type(1) == api::LuaValueType::kNumber) {
      state->SetTop(1);
      return 1;
    } else {
      auto[s, ok]=state->ToStringX(1);
      if (ok && state->StringToNumber(s))
        return 1;
    }
  } else {
    state->CheckType(1, api::LuaValueType::kString);
    auto s = utils::TrimSpace(state->ToString(1));
    auto base = state->CheckInteger(2);
    state->ArgCheck(2 <= base && base <= 36, 2, "base out of range");
    try {
      auto n = std::stol(s, nullptr, base);
      state->PushInteger(n);
      return 1;
    } catch (const std::exception &) {}
  }
  state->PushNil();
  return 1;
}
}
}