//
// Created by 方泓睿 on 2020/4/17.
//

#include <embryo/state/lua_state.h>
#include <embryo/api/lua_state.h>

namespace embryo::api {
std::shared_ptr<LuaState> LuaState::New() {
  return std::make_shared<state::LuaState>(kLuaMinStack);
}
}