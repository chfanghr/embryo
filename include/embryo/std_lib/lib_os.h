//
// Created by 方泓睿 on 2020/4/21.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_OS_H_
#define EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_OS_H_

#include <embryo/api/lua_state.h>

namespace embryo::std_lib {
extern api::FuncReg kOsLib;
int32_t OpenOsLib(const std::shared_ptr<api::LuaState> &);

namespace os {
int32_t Clock(const std::shared_ptr<api::LuaState> &);

int32_t DiffTime(const std::shared_ptr<api::LuaState> &);

int32_t Time(const std::shared_ptr<api::LuaState> &);

int32_t Date(const std::shared_ptr<api::LuaState> &);

int32_t Remove(const std::shared_ptr<api::LuaState> &);

int32_t Rename(const std::shared_ptr<api::LuaState> &);

int32_t TmpName(const std::shared_ptr<api::LuaState> &);

int32_t GetEnv(const std::shared_ptr<api::LuaState> &);

int32_t Execute(const std::shared_ptr<api::LuaState> &);

int32_t Exit(const std::shared_ptr<api::LuaState> &);

int32_t SetLocale(const std::shared_ptr<api::LuaState> &);
}
}
#endif //EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_OS_H_
