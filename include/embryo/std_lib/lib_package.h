//
// Created by 方泓睿 on 2020/4/24.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_PACKAGE_H_
#define EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_PACKAGE_H_

#include <embryo/api/lua_state.h>

namespace embryo::std_lib {
extern api::FuncReg kPackageLib;
extern api::FuncReg kLlFuncs;

int32_t OpenPackageLib(const std::shared_ptr<api::LuaState> &state);

namespace package {
int32_t SearchPath(const std::shared_ptr<api::LuaState> &);

int32_t Require(const std::shared_ptr<api::LuaState> &);
}
}

#endif //EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_PACKAGE_H_
