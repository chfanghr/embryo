//
// Created by 方泓睿 on 2020/4/8.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_API_LUA_STATE_H_
#define EMBRYO_INCLUDE_EMBRYO_API_LUA_STATE_H_

#include <embryo/api/basic_api.h>
#include <embryo/api/aux_lib.h>

namespace embryo::api {
class LuaState : public BasicApi, public AuxLib {
 public:
  static std::shared_ptr<LuaState> New();
};
}

#endif //EMBRYO_INCLUDE_EMBRYO_API_LUA_STATE_H_
