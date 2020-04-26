//
// Created by 方泓睿 on 2020/4/9.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_API_LUA_VM_H_
#define EMBRYO_INCLUDE_EMBRYO_API_LUA_VM_H_

#include <embryo/api/lua_state.h>

namespace embryo::api {
class LuaVM : public LuaState {
 public:
  [[nodiscard]] virtual int32_t PC() const = 0;
  virtual void AddPC(int32_t) = 0;
  virtual uint32_t Fetch() = 0;
  virtual void GetConst(int32_t idx) = 0;
  virtual void GetRK(int32_t idx) = 0;
  [[nodiscard]] virtual int32_t RegisterCount() const = 0;
  virtual void LoadVarArg(int32_t n) = 0;
  virtual void LoadProto(int32_t idx) = 0;
  virtual void CloseUpValues(int32_t a) = 0;
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_API_LUA_VM_H_
