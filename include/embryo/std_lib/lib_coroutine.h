//
// Created by 方泓睿 on 2020/4/24.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_COROUTINE_H_
#define EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_COROUTINE_H_

#include <embryo/api/lua_state.h>

namespace embryo::std_lib {
extern api::FuncReg kCoroutineLib;

int32_t OpenCoroutineLib(const std::shared_ptr<api::LuaState> &);

namespace coroutine {
int32_t Create(const std::shared_ptr<api::LuaState> &);

int32_t Resume(const std::shared_ptr<api::LuaState> &);

int32_t Status(const std::shared_ptr<api::LuaState> &);

int32_t Yield(const std::shared_ptr<api::LuaState> &);

int32_t IsYieldable(const std::shared_ptr<api::LuaState> &);

int32_t Running(const std::shared_ptr<api::LuaState> &);

int32_t Wrap(const std::shared_ptr<api::LuaState> &);
}
}
#endif //EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_COROUTINE_H_
