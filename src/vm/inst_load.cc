//
// Created by 方泓睿 on 2020/4/9.
//

#include <embryo/vm/inst_load.h>

namespace embryo::vm {
void LoadNil(Instruction ins, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] =ins.ABC();
  ++a;

  vm->PushNil();
  for (int64_t i = a; i <= a + b; i++)
    vm->Copy(-1, i);
  vm->Pop(1);
}

void LoadBool(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] =i.ABC();
  ++a;
  vm->PushBoolean(b != 0);
  vm->Replace(a);
  if (c != 0)
    vm->AddPC(1);
}

void LoadK(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, bx]=i.ABx();
  ++a;

  vm->GetConst(bx);
  vm->Replace(a);
}

void LoadKx(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, bx]=i.ABx();
  ++a;
  auto ax = Instruction(vm->Fetch()).Ax();

  vm->GetConst(ax);
  vm->Replace(a);
}
}