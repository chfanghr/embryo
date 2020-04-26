//
// Created by 方泓睿 on 2020/4/8.
//

#include <cmath>

#include <embryo/state/lua_state.h>
#include <embryo/state/lua_value.h>
#include <embryo/utils/inspection.h>
#include <embryo/utils/numbers.h>
#include <embryo/state/lua_table.h>
#include <embryo/bin_chunk/closure.h>

namespace embryo::state {
api::LuaValueType TypeOf(const LuaValue &value) {
  if (!value)
    return api::LuaValueType::kNone;

  if (ContainsType<std::nullptr_t>(value))
    return api::LuaValueType::kNil;
  if (ContainsType<bool>(value))
    return api::LuaValueType::kBoolean;
  if (ContainsType<int64_t>(value))
    return api::LuaValueType::kNumber;
  if (ContainsType<double>(value))
    return api::LuaValueType::kNumber;
  if (ContainsType<std::string>(value))
    return api::LuaValueType::kString;
  if (ContainsType<LuaTablePtr>(value))
    return api::LuaValueType::kTable;
  if (ContainsType<LuaClosurePtr>(value))
    return api::LuaValueType::kFunction;
  if (ContainsType<LuaStatePtr>(value))
    return api::LuaValueType::kThread;

  throw std::runtime_error(MSG_WITH_FUNC_NAME("unsupported lua value type"));
}

bool ToBoolean(const LuaValue &val) {
  switch (TypeOf(val)) {
    case api::LuaValueType::kNone:
    case api::LuaValueType::kNil:return false;
    case api::LuaValueType::kBoolean:return std::get<bool>(val.value());
    default:return true;
  }
}

std::tuple<double, bool> ToFloat(const LuaValue &val) {
  switch (TypeOf(val)) {
    case api::LuaValueType::kNumber:
      if (std::holds_alternative<LuaNumber>(val.value()))
        return {(double) std::get<LuaNumber>(val.value()), true};
      if (std::holds_alternative<LuaInteger>(val.value()))
        return {(double) std::get<LuaInteger>(val.value()), true};
    case api::LuaValueType::kString: //
    {
      {
        auto[num, ok]=utils::ParseInt(std::get<std::string>(val.value()));
        if (ok)
          return {(double) num, true};
      }
      {
        auto[num, ok]=utils::ParseFloat(std::get<std::string>(val.value()));
        if (ok)
          return {(double) num, true};
      }
    }
    default:return {0, false};
  }
}

std::tuple<int64_t, bool> ToInteger(const LuaValue &val) {
  switch (TypeOf(val)) {
    case api::LuaValueType::kNumber:
      if (std::holds_alternative<LuaNumber>(val.value()))
        return {(int64_t) std::get<LuaNumber>(val.value()), true};
      if (std::holds_alternative<LuaInteger>(val.value()))
        return {(int64_t) std::get<LuaInteger>(val.value()), true};
    case api::LuaValueType::kString: //
    {
      {
        auto[num, ok]=utils::ParseInt(std::get<std::string>(val.value()));
        if (ok)
          return {(int64_t) num, true};
      }
      {
        auto[num, ok]=utils::ParseFloat(std::get<std::string>(val.value()));
        if (ok)
          return {(int64_t) num, true};
      }
    }
    default:return {0, false};
  }
}

std::tuple<std::string, bool> ToString(const LuaValue &val) {
  switch (TypeOf(val)) {
    case api::LuaValueType::kString:return {std::get<std::string>(val.value()), true};
    case api::LuaValueType::kNumber: {
      std::string str{};
      if (std::holds_alternative<int64_t>(val.value()))
        str = fmt::format("{}", std::get<int64_t>(val.value()));
      if (std::holds_alternative<double>(val.value()))
        str = fmt::format("{}", std::get<double>(val.value()));
      return {str, true};
    }
    default:return {"", false};
  }
}

std::tuple<std::optional<int64_t>,
           std::optional<double>> OptionalNumber(const LuaValue &val) noexcept {
  auto[val_i, ok_i] = TryCast<LuaInteger>(val);
  auto[val_d, ok_d] = TryCast<LuaNumber>(val);
  if (ok_i)
    return {val_i, {}};
  if (ok_d)
    return {{}, val_d};
  return {{},
          {}};
}

LuaValue ToLuaValue(const bin_chunk::LuaConstant &c) {
  {
    auto[val, ok]= bin_chunk::TryCast<std::nullptr_t>(c);
    if (ok)
      return {val};
  }
  {
    auto[val, ok] = bin_chunk::TryCast<bool>(c);
    if (ok)
      return {val};
  }
  {
    auto[val, ok] = bin_chunk::TryCast<int64_t>(c);
    if (ok)
      return {val};
  }
  {
    auto[val, ok] = bin_chunk::TryCast<double>(c);
    if (ok)
      return {val};
  }
  {
    auto[val, ok] = bin_chunk::TryCast<std::string>(c);
    if (ok)
      return {val};
  }

  throw std::runtime_error(MSG_WITH_FUNC_NAME("unsupported constant type"));
}

std::string Stringify(const LuaValue &value) {
  switch (TypeOf(value)) {
    case api::LuaValueType::kNil:return "null";
    case api::LuaValueType::kNumber: {
      auto[str, ok]=ToString(value);
      return str;
    }
    case api::LuaValueType::kString: {
      auto[str, ok]=ToString(value);
      return AS_STRING(str);
    }
    case api::LuaValueType::kBoolean:return Cast<bool>(value) ? "true" : "false";
    case api::LuaValueType::kTable:return Cast<LuaTablePtr>(value)->String();
    default:return AS_STRING("<unknown>");
  }
}

bool IsNan(const LuaValue &val) {
  if (!val || !ContainsType<double>(val))
    return false;
  return std::isnan(Cast<double>(val));
}

bool IsNil(const LuaValue &v) {
  return ContainsType<std::nullptr_t>(v);
}

bool IsNone(const LuaValue &v) {
  return !v;
}

bool ContainsSharedPtr(const LuaValue &value) {
  return ContainsType<LuaAnyTypePtr>(value);
}

template<>
std::tuple<LuaTablePtr, bool> TryCast<LuaTablePtr>(
    const LuaValue &val) noexcept {
  auto[ptr, ok] = TryCast<LuaAnyTypePtr>(val);
  return {
      std::dynamic_pointer_cast<LuaTable>(ptr),
      ok && std::dynamic_pointer_cast<LuaTable>(ptr)
  };
}

template<>
bool ContainsType<LuaTablePtr>(const LuaValue &val) noexcept {
  auto[ptr, ok] = TryCast<LuaTablePtr>(val);
  return ok;
}

template<>
LuaTablePtr Cast<LuaTablePtr>(const LuaValue &val) {
  auto[ptr, ok] = TryCast<LuaTablePtr>(val);
  if (!ok || !ptr)
    throw std::runtime_error(MSG_WITH_FUNC_NAME("val doesnt contain std::shared_ptr<LuaTable>"));
  return ptr;
}

template<>
std::tuple<LuaClosurePtr, bool> TryCast<LuaClosurePtr>(
    const LuaValue &val) noexcept {
  auto[ptr, ok] = TryCast<LuaAnyTypePtr>(val);
  return {
      std::dynamic_pointer_cast<LuaClosure>(ptr),
      ok && std::dynamic_pointer_cast<LuaClosure>(ptr)
  };
}

template<>
LuaClosurePtr Cast<LuaClosurePtr>(const LuaValue &val) {
  auto[ptr, ok] = TryCast<LuaClosurePtr>(val);
  if (!ok || !ptr)
    throw std::runtime_error(MSG_WITH_FUNC_NAME("val doesnt contain std::shared_ptr<LuaClosurePtr>"));
  return ptr;
}

template<>
bool ContainsType<LuaClosurePtr>(const LuaValue &val) noexcept {
  auto[ptr, ok] = TryCast<LuaClosurePtr>(val);
  return ok;
}

template<>
std::tuple<NativeFunction, bool> TryCast<NativeFunction>(const LuaClosurePtr &c) noexcept {
  return {c->native_function_, c->native_function_ != nullptr};
}

template<>
std::tuple<std::shared_ptr<bin_chunk::Prototype>,
           bool> TryCast<std::shared_ptr<bin_chunk::Prototype>>(const LuaClosurePtr &c) noexcept {
  return {c->proto_, c->proto_ != nullptr};
}

template<>
NativeFunction Cast<NativeFunction>(const LuaClosurePtr &c) {
  auto[f, ok]=TryCast<NativeFunction>(c);
  if (ok)
    return f;
  throw std::runtime_error(
      MSG_WITH_FUNC_NAME("given closure object doesnt contain NativeFunction"));
}

template<>
std::shared_ptr<bin_chunk::Prototype> Cast<
    std::shared_ptr<bin_chunk::Prototype>>(const LuaClosurePtr &c) {
  auto[f, ok]=TryCast<std::shared_ptr<bin_chunk::Prototype>>(c);
  if (ok)
    return f;
  throw std::runtime_error(
      MSG_WITH_FUNC_NAME("given closure object doesnt contain std::shared_ptr<bin_chunk::Prototype>"));
}

template<>
bool ContainsType<std::shared_ptr<bin_chunk::Prototype>>(const LuaClosurePtr &c) noexcept {
  auto[f, ok]=TryCast<std::shared_ptr<bin_chunk::Prototype>>(c);
  return ok;
}

template<>
bool ContainsType<NativeFunction>(const LuaClosurePtr &c) noexcept {
  auto[f, ok]=TryCast<NativeFunction>(c);
  return ok;
}

template<>
NativeFunction Cast<NativeFunction>(const LuaValue &v) {
  return Cast<NativeFunction>(Cast<LuaClosurePtr>(v));
}

template<>
std::shared_ptr<bin_chunk::Prototype> Cast<
    std::shared_ptr<bin_chunk::Prototype>>(const LuaValue &v) {
  return Cast<
      std::shared_ptr<bin_chunk::Prototype>>(Cast<LuaClosurePtr>(v));
}

template<>
std::tuple<NativeFunction, bool> TryCast<NativeFunction>(const LuaValue &v) noexcept {
  auto[c, ok_c] = TryCast<LuaClosurePtr>(v);
  if (!ok_c)
    return {nullptr, false};
  return TryCast<NativeFunction>(c);
}

template<>
std::tuple<std::shared_ptr<bin_chunk::Prototype>,
           bool> TryCast<std::shared_ptr<bin_chunk::Prototype>>(const LuaValue &v) noexcept {
  auto[c, ok_c] = TryCast<LuaClosurePtr>(v);
  if (!ok_c)
    return {nullptr, false};
  return TryCast<std::shared_ptr<bin_chunk::Prototype>>(c);
}

template<>
bool ContainsType<std::shared_ptr<bin_chunk::Prototype>>(const LuaValue &v) noexcept {
  auto[c, ok_c] = TryCast<LuaClosurePtr>(v);
  if (!ok_c)
    return false;
  return ContainsType<std::shared_ptr<bin_chunk::Prototype>>(c);
}

template<>
bool ContainsType<NativeFunction>(const LuaValue &v) noexcept {
  auto[c, ok_c] = TryCast<LuaClosurePtr>(v);
  if (!ok_c)
    return false;
  return ContainsType<NativeFunction>(c);
}

bool IsNativeFunction(const LuaClosurePtr &c) {
  return ContainsType<NativeFunction>(c);
}

UpValue::UpValue(embryo::state::LuaValue val) : val_(std::move(val)) {}

std::shared_ptr<LuaTable> GetMetaTable(const LuaValue &val, const std::shared_ptr<LuaState> &state) {
  auto[t, ok]=TryCast<LuaTablePtr>(val);
  if (ok)
    return t->meta_table_;
  auto key = fmt::format("_MT{}", int32_t(TypeOf(val)));
  auto mt = state->registry_->Get(key);
  if (mt)
    return Cast<LuaTablePtr>(mt);
  return nullptr;
}

void SetMetaTable(const LuaValue &val, const std::shared_ptr<LuaTable> &mt, const std::shared_ptr<LuaState> &state) {
  auto[t, ok]=TryCast<LuaTablePtr>(val);
  if (ok) {
    t->meta_table_ = mt;
    return;
  }
  auto key = fmt::format("_MT{}", int32_t(TypeOf(val)));
  state->registry_->Put(key, mt);
}

LuaValue CallMetaMethod(const LuaValue &a,
                        const LuaValue &b,
                        const std::string &method,
                        const std::shared_ptr<LuaState> &state) {
  LuaValue mm{};

  if (!(mm = GetMetaField(a, method, state))) {
    if (!(mm = GetMetaField(b, method, state)))
      return {};
  }

  state->stack_->Check(4);
  state->stack_->Push(mm);
  state->stack_->Push(a);
  state->stack_->Push(b);
  state->Call(2, 1);
  return state->stack_->Pop();
}

LuaValue GetMetaField(const LuaValue &val, const std::string &field_name, const std::shared_ptr<LuaState> &state) {
  auto mt = GetMetaTable(val, state);
  if (mt)
    return mt->Get(field_name);
  return {};
}

template<>
bool ContainsType<LuaStatePtr>(const LuaValue &val) noexcept {
  return std::get<1>(TryCast<LuaStatePtr>(val));
}

template<>
LuaStatePtr Cast<LuaStatePtr>(const LuaValue &val) {
  auto[ptr, ok]=TryCast<LuaStatePtr>(val);
  if (!ok || !ptr)
    throw std::runtime_error(MSG_WITH_FUNC_NAME("val doesnt contain std::shared_ptr<LuaStatePtr>"));
  return ptr;
}

template<>
std::tuple<LuaStatePtr, bool> TryCast<LuaStatePtr>(const LuaValue &val) noexcept {
  auto[ptr, ok]=TryCast<LuaAnyTypePtr>(val);
  return {
      std::dynamic_pointer_cast<LuaState>(ptr),
      std::dynamic_pointer_cast<LuaState>(ptr) != nullptr
  };
}
}