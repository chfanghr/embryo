//
// Created by 方泓睿 on 2020/4/9.
//

#include <stdexcept>

#include <embryo/utils/inspection.h>
#include <embryo/vm/inst_misc.h>

namespace embryo::vm {
void Move(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;
  ++b;

  vm->Copy(b, a);
}

void Jmp(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, s_bx]= i.AsBx();
  vm->AddPC(s_bx);
  if (a != 0) // TODO
    vm->CloseUpValues(a);
}
}