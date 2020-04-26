//
// Created by 方泓睿 on 2020/4/22.
//

#include <vector>

#include <embryo/utils/types.h>
#include <embryo/utils/strings.h>
#include <embryo/std_lib/lib_string.h>
#include <embryo/utils/bytes.h>
#include <embryo/utils/inspection.h>

namespace embryo::std_lib {
namespace {
void CreateMetatable(const std::shared_ptr<api::LuaState> &state) {
  state->CreateTable(0, 1);
  state->PushString("dummy");
  state->PushValue(-2);
  state->SetMetatable(-2);
  state->Pop(1);
  state->PushValue(-2);
  state->SetField(-2, "__index");
  state->Pop(1);
}

size_t PosRelat(int64_t pos, size_t len) {
  if (pos >= 0)
    return pos;
  if (-pos > len)
    return 0;
  return len + pos + 1;
}

//std::string FmtArg(const std::string &tag, const std::shared_ptr<api::LuaState> &state, size_t arg_idx) {
//  THROW_NOT_IMPLEMENTED;
//}
}

api::FuncReg kStringLib{
    {"len", string::Len},
    {"rep", string::Rep},
    {"reverse", string::Reverse},
    {"lower", string::Lower},
    {"upper", string::Upper},
    {"Sub", string::Sub},
    {"byte", string::Byte},
    {"char", string::Char},
    {"dump", string::Dump},
    {"format", string::Format},
    {"packsize", string::PackSize},
    {"pack", string::Pack},
    {"unpack", string::Unpack},
    {"find", string::Find},
    {"match", string::Match},
    {"gsub", string::GSub},
    {"gmatch", string::GMatch}
};

int32_t OpenStringLib(const std::shared_ptr<api::LuaState> &state) {
  state->NewLib(kStringLib);
  CreateMetatable(state);
  return 1;
}

namespace string {
int32_t Len(const std::shared_ptr<api::LuaState> &state) {
  auto s = state->CheckString(1);
  state->PushInteger(s.size());
  return 1;
}

int32_t Rep(const std::shared_ptr<api::LuaState> &state) {
  auto s = state->CheckString(1);
  auto n = state->CheckInteger(2);
  auto sep = state->OptString(3, "");

  if (n <= 0)
    state->PushString("");
  else if (n == 1)
    state->PushString(s);
  else
    state->PushString(utils::Join(std::vector<std::string>(n, s), sep));

  return 1;
}

int32_t Reverse(const std::shared_ptr<api::LuaState> &state) {
  auto s = state->CheckString(1);
  state->PushString(std::string(s.rbegin(), s.rend()));
  return 1;
}

int32_t Lower(const std::shared_ptr<api::LuaState> &state) {
  auto s = state->CheckString(1);
  state->PushString(utils::ToLower(s));
  return 1;
}

int32_t Upper(const std::shared_ptr<api::LuaState> &state) {
  auto s = state->CheckString(1);
  state->PushString(utils::ToUpper(s));
  return 1;
}

int32_t Sub(const std::shared_ptr<api::LuaState> &state) {
  auto s = state->CheckString(1);
  auto s_len = s.size();
  auto i = PosRelat(state->CheckInteger(2), s_len);
  auto j = PosRelat(state->OptInteger(3, -1), s_len);

  if (i < 1)
    i = 1;
  if (j > s_len)
    j = s_len;
  if (i <= j)
    state->PushString(std::string(s.begin() + i - 1, s.begin() + j));
  else
    state->PushString("");
  return 1;
}

int32_t Byte(const std::shared_ptr<api::LuaState> &state) {
  auto s = state->CheckString(1);
  auto s_len = s.size();
  auto i = PosRelat(state->OptInteger(2, 1), s_len);
  auto j = PosRelat(state->OptInteger(3, i), s_len);

  if (i < 1) i = 1;
  if (j > s_len) j = s_len;

  if (i > j) return 0;

  auto n = j - i + 1;
  state->CheckStack2(n, "string slice too long");

  for (size_t k = 0; k < n; k++)
    state->PushInteger(s[i + k - 1]);

  return n;
}

int32_t Char(const std::shared_ptr<api::LuaState> &state) {
  auto n_args = state->GetTop();
  std::vector<embryo::Byte> s(n_args);
  for (size_t i = 1; i <= n_args; ++i) {
    auto c = state->CheckInteger(i);
    state->ArgCheck(embryo::Byte(c) == c, i, "value out of range");
    s[i - 1] = c;
  }

  state->PushString(utils::Convert(s));
  return 1;
}

int32_t Dump(const std::shared_ptr<api::LuaState> &state) {
  // TODO
  THROW_NOT_IMPLEMENTED;
}

int32_t Format(const std::shared_ptr<api::LuaState> &state) {
  auto fmt_str = state->CheckString(1);
  if (fmt_str.size() <= 1 || fmt_str.find('%') == std::string::npos) {
    state->PushString(fmt_str);
    return 1;
  }

  THROW_NOT_IMPLEMENTED;
}

int32_t PackSize(const std::shared_ptr<api::LuaState> &state) {
  // TODO
  THROW_NOT_IMPLEMENTED;
}

int32_t Pack(const std::shared_ptr<api::LuaState> &state) {
  // TODO
  THROW_NOT_IMPLEMENTED;
}

int32_t Unpack(const std::shared_ptr<api::LuaState> &state) {
  // TODO
  THROW_NOT_IMPLEMENTED;
}

int32_t Find(const std::shared_ptr<api::LuaState> &state);

int32_t Match(const std::shared_ptr<api::LuaState> &state);

int32_t GSub(const std::shared_ptr<api::LuaState> &state);

int32_t GMatch(const std::shared_ptr<api::LuaState> &state);
}
}