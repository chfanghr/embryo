//
// Created by 方泓睿 on 2020/4/19.
//

#include <fmt/format.h>

#include <embryo/utils/io.h>
#include <embryo/state/lua_state.h>
#include <embryo/utils/bytes.h>
#include <embryo/std_lib/lib_basic.h>
#include <embryo/std_lib/lib_math.h>

namespace embryo::state {
int32_t LuaState::Error2(const std::string &msg) {
  PushString(msg); // TODO
  return Error();
}

int32_t LuaState::ArgError(int32_t arg, const std::string &extra_message) {
  return Error2(fmt::format("bad argument #{} ({})", arg, extra_message));
}

void LuaState::CheckStack2(int32_t sz, const std::string &msg) {
  if (!CheckStack(sz))
    Error2(fmt::format("stack overflow ({})", msg));
}

void LuaState::ArgCheck(bool cond, int32_t arg, const std::string &extra_msg) {
  if (!cond)
    ArgError(arg, extra_msg);
}

void LuaState::CheckAny(int32_t arg) {
  if (Type(arg) == api::LuaValueType::kNone)
    ArgError(arg, "value expected");
}

void LuaState::CheckType(int32_t arg, api::LuaValueType type) {
  if (Type(arg) != type)
    TagError(arg, type);
}

void LuaState::TagError(int32_t arg, embryo::api::LuaValueType tag) {
  TypeError(arg, TypeName(tag));
}

int32_t LuaState::TypeError(int32_t arg, const std::string &t_name) {
  std::string type_arg{};

  if (GetMetaField(arg, "__name") == api::LuaValueType::kString)
    type_arg = ToString(-1);
  else if (Type(arg) == api::LuaValueType::kLightUserData)
    type_arg = "light user data";
  else
    type_arg = TypeName2(arg);
  auto msg = t_name + "expected, got" + type_arg;
  PushString(msg);
  return ArgError(arg, msg);
}

int64_t LuaState::CheckInteger(int32_t arg) {
  auto[i, ok]=ToIntegerX(arg);
  if (!ok)
    IntError(arg);
  return i;
}

void LuaState::IntError(int32_t arg) {
  if (!IsNumber(arg))
    ArgError(arg, "number has no integer representation");
  else
    TagError(arg, api::LuaValueType::kNumber);
}

double LuaState::CheckNumber(int32_t arg) {
  auto[d, ok]=ToNumberX(arg);
  if (!ok)
    TagError(arg, api::LuaValueType::kNumber);
  return d;
}

std::string LuaState::CheckString(int32_t arg) {
  auto[s, ok] = ToStringX(arg);
  if (!ok)
    TagError(arg, api::LuaValueType::kString);
  return s;
}

int64_t LuaState::OptInteger(int32_t arg, int64_t d) {
  if (IsNoneOrNil(arg))
    return d;
  return CheckInteger(arg);
}

double LuaState::OptNumber(int32_t arg, double d) {
  if (IsNoneOrNil(arg))
    return d;
  return CheckNumber(arg);
}

std::string LuaState::OptString(int32_t arg, const std::string &d) {
  if (IsNoneOrNil(arg))
    return d;
  return CheckString(arg);
}

bool LuaState::DoFile(const std::string &file) {
  return LoadFile(file) != api::Status::kOk ||
      PCall(0, kMultert, 0) != api::Status::kOk;
}

bool LuaState::DoString(const std::string &str) {
  return LoadString(str) != api::Status::kOk ||
      PCall(0, kMultert, 0) != api::Status::kOk;
}

api::Status LuaState::LoadFile(const std::string &file) {
  return LoadFileX(file, "bt");
}

api::Status LuaState::LoadFileX(const std::string &file, const std::string &mode) {
  try {
    auto data = utils::ReadFile<std::vector<Byte>>(file);
    Load(data, "@" + file, mode);
    return api::Status::kOk;
  } catch (const std::exception &exp) {}
  return api::Status::kErrFile;
}

api::Status LuaState::LoadString(const std::string &s) {
  return Load(utils::Convert(s), s, "bt");
}

std::string LuaState::TypeName2(int64_t idx) {
  return TypeName(Type(idx));
}

std::string LuaState::ToString2(int64_t idx) {
  if (CallMeta(idx, "__tostring")) {
    if (!IsString(-1))
      Error2("'__tostring' must return a string");
  } else {
    switch (Type(idx)) {
      case api::LuaValueType::kNil: {
        PushString("nil");
        break;
      }
      case api::LuaValueType::kBoolean: {
        if (ToBoolean(idx))
          PushString("true");
        else
          PushString("false");
        break;
      }
      case api::LuaValueType::kNumber: {
        if (IsInteger(idx))
          PushString(fmt::format("{}", ToInteger(idx)));
        else
          PushString(fmt::format("{}", ToNumber(idx)));
        break;
      }
      case api::LuaValueType::kString: {
        PushValue(idx);
        break;
      }
      default: {
        auto tt = GetMetaField(idx, "__name");
        std::string kind{};
        if (tt == api::LuaValueType::kString)
          kind = CheckString(-1);
        else
          kind = TypeName2(idx);

        PushString(fmt::format("{}: {}", kind, ToPointer(idx)));
        if (tt != api::LuaValueType::kNil)
          Remove(-2);
      }
    }
  }

  return CheckString(-1);
}

int64_t LuaState::Len2(int64_t idx) {
  Len(idx);
  auto[i, is_num]=ToIntegerX(-1);
  if (!is_num)
    Error2("object length is not an integer");
  Pop(1);
  return i;
}

bool LuaState::GetSubTable(int64_t idx, const std::string &f_name) {
  if (GetField(idx, f_name) == api::LuaValueType::kTable)
    return true;
  Pop(1);
  idx = stack_->AbsIndex(idx);
  NewTable();
  PushValue(-1);
  SetField(idx, f_name);
  return false;
}

api::LuaValueType LuaState::GetMetaField(int32_t obj, const std::string &e) {
  if (!GetMetatable(obj))
    return api::LuaValueType::kNil;

  PushString(e);
  auto tt = RawGet(-2);
  if (tt == api::LuaValueType::kNil)
    Pop(2);
  else
    Remove(-2);
  return tt;
}

bool LuaState::CallMeta(int32_t obj, const std::string &e) {
  obj = stack_->AbsIndex(obj);
  if (GetMetaField(obj, e) == api::LuaValueType::kNil)
    return false;
  PushValue(obj);
  Call(1, 1);
  return true;
}

void LuaState::OpenLibs() {
  api::FuncReg libs{
      {"_G", std_lib::OpenBaseLib},
      // TODO
  };

  for (const auto &[name, func] : libs) {
    RequireF(name, func, true);
    Pop(1);
  }
}

void LuaState::RequireF(const std::string &mod_name, const NativeFunction &open_f, bool glb) {
  GetSubTable(kLuaRegistryIndex, "_LOADED");
  GetField(-1, mod_name);
  if (!ToBoolean(-1)) {
    Pop(1);
    PushNativeFunction(open_f);
    PushString(mod_name);
    Call(1, 1);
    PushValue(-1);
    SetField(-3, mod_name);
  }
  Remove(-2);
  if (glb) {
    PushValue(-1);
    SetGlobal(mod_name);
  }
}

void LuaState::NewLib(const api::FuncReg &reg) {
  NewLibTable(reg);
  SetFuncs(reg, 0);
}

void LuaState::NewLibTable(const api::FuncReg &reg) {
  CreateTable(0, reg.size());
}

void LuaState::SetFuncs(const api::FuncReg &reg, int32_t n_up) {
  CheckStack2(n_up, "too many up values");
  for (const auto &[name, func]:reg) {
    for (size_t i = 0; i < n_up; i++)
      PushValue(-n_up);
    PushNativeClosure(func, n_up);
    SetField(-(n_up + 2), name);
  }
  Pop(n_up);
}
}