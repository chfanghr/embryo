//
// Created by 方泓睿 on 2020/4/20.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_MATH_H_
#define EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_MATH_H_

#include <limits>
#include <embryo/api/lua_state.h>

namespace embryo::std_lib {
extern api::FuncReg kMathLib;

int32_t OpenMathLib(const std::shared_ptr<api::LuaState> &);

namespace math {
int32_t Random(const std::shared_ptr<api::LuaState> &);

int32_t RandomSeed(const std::shared_ptr<api::LuaState> &);

int32_t Max(const std::shared_ptr<api::LuaState> &);

int32_t Min(const std::shared_ptr<api::LuaState> &);

int32_t Exp(const std::shared_ptr<api::LuaState> &);

int32_t Log(const std::shared_ptr<api::LuaState> &);

int32_t Deg(const std::shared_ptr<api::LuaState> &);

int32_t Rad(const std::shared_ptr<api::LuaState> &);

int32_t Sin(const std::shared_ptr<api::LuaState> &);

int32_t Cos(const std::shared_ptr<api::LuaState> &);

int32_t Tan(const std::shared_ptr<api::LuaState> &);

int32_t Asin(const std::shared_ptr<api::LuaState> &);

int32_t Acos(const std::shared_ptr<api::LuaState> &);

int32_t Atan(const std::shared_ptr<api::LuaState> &);

int32_t Ceil(const std::shared_ptr<api::LuaState> &);

int32_t Floor(const std::shared_ptr<api::LuaState> &);

int32_t FMod(const std::shared_ptr<api::LuaState> &);

int32_t ModF(const std::shared_ptr<api::LuaState> &);

int32_t Abs(const std::shared_ptr<api::LuaState> &);

int32_t Sqrt(const std::shared_ptr<api::LuaState> &);

int32_t Ult(const std::shared_ptr<api::LuaState> &);

int32_t ToInteger(const std::shared_ptr<api::LuaState> &);

int32_t Type(const std::shared_ptr<api::LuaState> &);

const int64_t kMaxInteger = std::numeric_limits<int64_t>::max();
const int64_t kMinInteger = std::numeric_limits<int64_t>::lowest();
const double kHuge = std::numeric_limits<double>::infinity();
}
}
#endif //EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_MATH_H_
