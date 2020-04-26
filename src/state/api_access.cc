//
// Created by 方泓睿 on 2020/4/8.
//

#include <fmt/format.h>

#include <embryo/state/lua_state.h>

namespace embryo::state {
std::string LuaState::TypeName(api::LuaValueType type) const {
  switch (type) {
    case api::LuaValueType::kNil: return "nil";
    case api::LuaValueType::kBoolean: return "boolean";
    case api::LuaValueType::kUserData:
    case api::LuaValueType::kLightUserData: return "user_data";
    case api::LuaValueType::kNumber:return "number";
    case api::LuaValueType::kString:return "string";
    case api::LuaValueType::kTable:return "table";
    case api::LuaValueType::kFunction:return "function";
    case api::LuaValueType::kThread:return "thread";
    case api::LuaValueType::kNone:return "none";
  }

  Error("invalid type");
}

api::LuaValueType LuaState::Type(int64_t idx) const {
  return TypeOf(stack_->Get(idx));
}

bool LuaState::IsNone(int64_t idx) const {
  return Type(idx) == api::LuaValueType::kNone;
}

bool LuaState::IsNil(int64_t idx) const {
  return Type(idx) == api::LuaValueType::kNil;
}

bool LuaState::IsNoneOrNil(int64_t idx) const {
  return IsNone(idx) || IsNil(idx);
}

bool LuaState::IsBoolean(int64_t idx) const {
  return Type(idx) == api::LuaValueType::kBoolean;
}

bool LuaState::IsString(int64_t idx) const {
  return Type(idx) == api::LuaValueType::kString || IsNumber(idx);
}

bool LuaState::IsFunction(int64_t idx) const {
  return Type(idx) == api::LuaValueType::kFunction;
}

bool LuaState::IsNumber(int64_t idx) const {
  auto[num, ok]=ToNumberX(idx);
  return ok;
}

bool LuaState::IsInteger(int64_t idx) const {
  if (stack_->Get(idx))
    return std::holds_alternative<int64_t>(stack_->Get(idx).value());
  return false;
}

LuaNumber LuaState::ToNumber(int64_t idx) const {
  auto[num, ok]=ToNumberX(idx);
  return num;
}

std::tuple<LuaNumber, bool> LuaState::ToNumberX(int64_t idx) const {
  return state::ToFloat(stack_->Get(idx));
}

LuaInteger LuaState::ToInteger(int64_t idx) const {
  auto[num, ok]=ToIntegerX(idx);
  return num;
}

std::tuple<LuaInteger, bool> LuaState::ToIntegerX(int64_t idx) const {
  return state::ToInteger(stack_->Get(idx));
}

std::string LuaState::ToString(int64_t idx) {
  auto[str, ok]=ToStringX(idx);
  return str;
}

std::tuple<std::string, bool> LuaState::ToStringX(int64_t idx) {
  switch (TypeOf(stack_->Get(idx))) {
    case api::LuaValueType::kString:return {Cast<std::string>(stack_->Get(idx)), true};
    case api::LuaValueType::kNumber: {
      std::string str{};
      if (ContainsType<int64_t>(stack_->Get(idx)))
        str = fmt::format("{}", Cast<int64_t>(stack_->Get(idx)));
      if (ContainsType<double>(stack_->Get(idx)))
        str = fmt::format("{}", Cast<double>(stack_->Get(idx)));
      stack_->Set(idx, str);
      return {str, true};
    }
    default:return {"", false};
  }
}

bool LuaState::ToBoolean(int64_t idx) const {
  return state::ToBoolean(stack_->Get(idx));
}

void *LuaState::ToPointer(int64_t idx) {
  return stack_->GetPointer(idx);
}

std::shared_ptr<api::LuaState> LuaState::ToThread(int64_t idx) {
  auto val = stack_->Get(idx);
  auto[state, ok]=TryCast<LuaStatePtr>(val);
  return state;
}
}