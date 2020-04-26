//
// Created by 方泓睿 on 2020/4/21.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_TABLE_H_
#define EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_TABLE_H_

#include <embryo/api/lua_state.h>

namespace embryo::std_lib {
extern api::FuncReg kTableLib;

int32_t OpenTableLib(const std::shared_ptr<api::LuaState> &);

namespace table {
int32_t Move(const std::shared_ptr<api::LuaState> &);

int32_t Insert(const std::shared_ptr<api::LuaState> &);

int32_t Remove(const std::shared_ptr<api::LuaState> &);

int32_t Sort(const std::shared_ptr<api::LuaState> &);

int32_t Concat(const std::shared_ptr<api::LuaState> &);

int32_t Pack(const std::shared_ptr<api::LuaState> &);

int32_t Unpack(const std::shared_ptr<api::LuaState> &);

const size_t kMaxLen = 1000000;
}

enum class TableOp : int32_t {
  kR = 1,
  kW = 2,
  kL = 4,
  kRW = (kR | kW) // NOLINT(hicpp-signed-bitwise)
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_STD_LIB_LIB_TABLE_H_
