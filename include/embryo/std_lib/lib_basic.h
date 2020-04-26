//
// Created by 方泓睿 on 2020/4/19.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_BASIC_H_
#define EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_BASIC_H_

#include <embryo/api/lua_state.h>

namespace embryo::std_lib {
extern api::FuncReg kBaseLib;
extern int64_t kRandomSeed;

int32_t OpenBaseLib(const std::shared_ptr<api::LuaState> &);

namespace base {
int32_t Print(const std::shared_ptr<api::LuaState> &);

int32_t Assert(const std::shared_ptr<api::LuaState> &);

int32_t Error(const std::shared_ptr<api::LuaState> &);

int32_t Select(const std::shared_ptr<api::LuaState> &);

int32_t IPairs(const std::shared_ptr<api::LuaState> &);

int32_t Pairs(const std::shared_ptr<api::LuaState> &);

int32_t Next(const std::shared_ptr<api::LuaState> &);

int32_t Load(const std::shared_ptr<api::LuaState> &);

int32_t LoadFile(const std::shared_ptr<api::LuaState> &);

int32_t DoFile(const std::shared_ptr<api::LuaState> &);

int32_t PCall(const std::shared_ptr<api::LuaState> &);

int32_t XPCall(const std::shared_ptr<api::LuaState> &);

int32_t GetMetatable(const std::shared_ptr<api::LuaState> &);

int32_t SetMetatable(const std::shared_ptr<api::LuaState> &);

int32_t RawEqual(const std::shared_ptr<api::LuaState> &);

int32_t RawLen(const std::shared_ptr<api::LuaState> &);

int32_t RawGet(const std::shared_ptr<api::LuaState> &);

int32_t RawSet(const std::shared_ptr<api::LuaState> &);

int32_t Type(const std::shared_ptr<api::LuaState> &);

int32_t ToString(const std::shared_ptr<api::LuaState> &);

int32_t ToNumber(const std::shared_ptr<api::LuaState> &);
}
}

#endif //EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_BASIC_H_
