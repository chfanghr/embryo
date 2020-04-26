//
// Created by 方泓睿 on 2020/4/10.
//

#include <embryo/vm/inst_table.h>
#include <embryo/utils/fpb.h>

namespace embryo::vm {
void NewTable(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c]= i.ABC();
  ++a;

  vm->CreateTable(utils::FbToInt(b), utils::FbToInt(c));
  vm->Replace(a);
}

void GetTable(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c]= i.ABC();
  ++a;
  ++b;

  vm->GetRK(c);
  vm->GetTable(b);
  vm->Replace(a);
}

void SetTable(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c]= i.ABC();
  ++a;

  vm->GetRK(b);
  vm->GetRK(c);
  vm->SetTable(a);
}

void SetList(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c]= i.ABC();
  ++a;

  if (c > 0)
    --c;
  else
    c = Instruction(vm->Fetch()).Ax();

  auto b_is_zero = b == 0;
  if (b_is_zero) {
    b = vm->ToInteger(-1) - a - 1;
    vm->Pop(1);
  }

  vm->CheckStack(1);
  int64_t idx = c * kNFieldPerFlush;
  for (int32_t j = 1; j <= b; j++) {
    ++idx;
    vm->PushValue(a + j);
    vm->SetI(a, idx);
  }

  if (b_is_zero) {
    for (int64_t j = vm->RegisterCount() + 1; j <= vm->GetTop(); j++) {
      ++idx;
      vm->PushValue(j);
      vm->SetI(a, idx);
    }

    vm->SetTop(vm->RegisterCount());
  }
}
}
