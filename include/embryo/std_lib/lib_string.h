//
// Created by 方泓睿 on 2020/4/22.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_STRING_H_
#define EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_STRING_H_

#include <embryo/api/lua_state.h>

namespace embryo::std_lib {
extern api::FuncReg kStringLib;

int32_t OpenStringLib(const std::shared_ptr<api::LuaState> &state);

namespace string {
int32_t Len(const std::shared_ptr<api::LuaState> &);

int32_t Rep(const std::shared_ptr<api::LuaState> &);

int32_t Reverse(const std::shared_ptr<api::LuaState> &);

int32_t Lower(const std::shared_ptr<api::LuaState> &);

int32_t Upper(const std::shared_ptr<api::LuaState> &);

int32_t Sub(const std::shared_ptr<api::LuaState> &);

int32_t Byte(const std::shared_ptr<api::LuaState> &);

int32_t Char(const std::shared_ptr<api::LuaState> &);

int32_t Dump(const std::shared_ptr<api::LuaState> &);

int32_t Format(const std::shared_ptr<api::LuaState> &);

int32_t PackSize(const std::shared_ptr<api::LuaState> &);

int32_t Pack(const std::shared_ptr<api::LuaState> &);

int32_t Unpack(const std::shared_ptr<api::LuaState> &);

int32_t Find(const std::shared_ptr<api::LuaState> &);

int32_t Match(const std::shared_ptr<api::LuaState> &);

int32_t GSub(const std::shared_ptr<api::LuaState> &);

int32_t GMatch(const std::shared_ptr<api::LuaState> &);
}
}

#endif //EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_STRING_H_
