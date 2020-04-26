//
// Created by 方泓睿 on 2020/4/11.
//

#include <embryo/state/lua_state.h>
#include <embryo/vm/inst_up_value.h>

namespace embryo::vm {
void GetTabUp(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;
  ++b;

  vm->GetRK(c);
  vm->GetTable(state::LuaUpValueIndex(b));
  vm->Replace(a);
}

void GetUpVal(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;
  ++b;

  vm->Copy(state::LuaUpValueIndex(b), a);
}

void SetUpVal(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;
  ++b;

  vm->Copy(a, state::LuaUpValueIndex(b));
}

void SetTabUp(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;

  vm->GetRK(b);
  vm->GetRK(c);
  vm->SetTable(state::LuaUpValueIndex(a));
}
}