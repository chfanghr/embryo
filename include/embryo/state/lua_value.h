//
// Created by 方泓睿 on 2020/4/8.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STATE_LUA_VALUE_H_
#define EMBRYO_INCLUDE_EMBRYO_STATE_LUA_VALUE_H_

#include <stdexcept>
#include <cstdlib>
#include <variant>
#include <string>
#include <optional>
#include <tuple>

#include <embryo/api/constants.h>
#include <embryo/utils/types.h>
#include <embryo/bin_chunk/bin_chunk.h>
#include <embryo/api/lua_state.h>

namespace embryo::bin_chunk {
class Closure;
}

namespace embryo::state {
struct LuaAnyType {
  virtual ~LuaAnyType() = default;
};

class LuaTable;
class LuaState;

using LuaAnyTypePtr = std::shared_ptr<LuaAnyType>;
using LuaTablePtr= std::shared_ptr<LuaTable>;
using LuaClosurePtr = std::shared_ptr<bin_chunk::Closure>;
using LuaClosure=bin_chunk::Closure;
using LuaStatePtr= std::shared_ptr<LuaState>;
using NativeFunction= api::NativeFunction;

using LuaValue=std::optional<
    std::variant<
        std::nullptr_t,
        bool,
        LuaInteger,
        LuaNumber,
        std::string,
        LuaAnyTypePtr>
>;

class UpValue {
 public:
  LuaValue val_;

  UpValue(LuaValue val);
};

// Try Cast
template<typename T>
std::tuple<T, bool> TryCast(const LuaValue &val) noexcept {
  if (!val)
    return {{}, false};
  if (std::holds_alternative<T>(val.value()))
    return {std::get<T>(val.value()), true};
  return {{}, false};
}

template<>
std::tuple<LuaTablePtr, bool> TryCast<LuaTablePtr>(const LuaValue &val) noexcept;

template<>
std::tuple<LuaClosurePtr, bool> TryCast<LuaClosurePtr>(const LuaValue &val) noexcept;

// Cast
template<typename T>
T Cast(const LuaValue &val) {
  return std::get<T>(val.value());
}

template<>
LuaTablePtr Cast<LuaTablePtr>(const LuaValue &val);

template<>
LuaClosurePtr Cast<LuaClosurePtr>(const LuaValue &val);

// ContainsType
template<typename T>
bool ContainsType(const LuaValue &val) noexcept {
  if (!val) return false;
  return std::holds_alternative<T>(val.value());
}

template<>
bool ContainsType<LuaTablePtr>(const LuaValue &val) noexcept;

template<>
bool ContainsType<LuaClosurePtr>(const LuaValue &val) noexcept;

api::LuaValueType TypeOf(const LuaValue &value);

bool ToBoolean(const LuaValue &);

std::tuple<double, bool> ToFloat(const LuaValue &);

std::tuple<int64_t, bool> ToInteger(const LuaValue &);

std::tuple<std::string, bool> ToString(const LuaValue &);

bool IsNan(const LuaValue &val);

std::tuple<std::optional<int64_t>,
           std::optional<double>> OptionalNumber(const LuaValue &val) noexcept;

LuaValue ToLuaValue(const bin_chunk::LuaConstant &);

std::string Stringify(const LuaValue &value);

bool IsNil(const LuaValue &v);

bool IsNone(const LuaValue &v);

bool ContainsSharedPtr(const LuaValue &value);

bool IsNativeFunction(const LuaClosurePtr &ptr);

template<typename T>
std::tuple<T, bool> TryCast(const LuaClosurePtr &) noexcept;

template<>
std::tuple<NativeFunction, bool> TryCast<NativeFunction>(const LuaClosurePtr &) noexcept;

template<>
std::tuple<std::shared_ptr<bin_chunk::Prototype>,
           bool> TryCast<std::shared_ptr<bin_chunk::Prototype>>(const LuaClosurePtr &) noexcept;

template<typename T>
T Cast(const LuaClosurePtr &);

template<>
NativeFunction Cast<NativeFunction>(const LuaClosurePtr &);

template<>
std::shared_ptr<bin_chunk::Prototype> Cast<
    std::shared_ptr<bin_chunk::Prototype>>(const LuaClosurePtr &);

template<typename T>
bool ContainsType(const LuaClosurePtr &) noexcept;

template<>
bool ContainsType<std::shared_ptr<bin_chunk::Prototype>>(const LuaClosurePtr &) noexcept;

template<>
bool ContainsType<NativeFunction>(const LuaClosurePtr &) noexcept;

template<>
NativeFunction Cast<NativeFunction>(const LuaValue &);

template<>
std::shared_ptr<bin_chunk::Prototype> Cast<
    std::shared_ptr<bin_chunk::Prototype>>(const LuaValue &);

template<>
std::tuple<NativeFunction, bool> TryCast<NativeFunction>(const LuaValue &) noexcept;

template<>
std::tuple<std::shared_ptr<bin_chunk::Prototype>,
           bool> TryCast<std::shared_ptr<bin_chunk::Prototype>>(const LuaValue &) noexcept;

template<>
bool ContainsType<std::shared_ptr<bin_chunk::Prototype>>(const LuaValue &) noexcept;

template<>
bool ContainsType<NativeFunction>(const LuaValue &) noexcept;

class LuaState;

std::shared_ptr<LuaTable> GetMetaTable(const LuaValue &val, const std::shared_ptr<LuaState> &state);

void SetMetaTable(const LuaValue &val, const std::shared_ptr<LuaTable> &mt, const std::shared_ptr<LuaState> &state);

LuaValue CallMetaMethod(const LuaValue &a,
                        const LuaValue &b,
                        const std::string &method,
                        const std::shared_ptr<LuaState> &state);

LuaValue GetMetaField(const LuaValue &val, const std::string &field_name, const std::shared_ptr<LuaState> &state);

template<>
bool ContainsType<LuaStatePtr>(const LuaValue &) noexcept;

template<>
LuaStatePtr Cast<LuaStatePtr>(const LuaValue &);

template<>
std::tuple<LuaStatePtr, bool> TryCast<LuaStatePtr>(const LuaValue &) noexcept;
}
#endif //EMBRYO_INCLUDE_EMBRYO_STATE_LUA_VALUE_H_
