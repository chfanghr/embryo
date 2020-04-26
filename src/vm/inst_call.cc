//
// Created by 方泓睿 on 2020/4/11.
//

#include <embryo/utils/inspection.h>
#include <embryo/vm/inst_call.h>

namespace embryo::vm {
namespace {
void FixStack(int32_t a, const std::shared_ptr<api::LuaVM> &vm) {
  int32_t x = vm->ToInteger(-1);
  vm->Pop(1);

  vm->CheckStack(x - a);
  for (int64_t i = a; i < x; i++)
    vm->PushValue(i);
  vm->Rotate(vm->RegisterCount() + 1, x - a);
}

int32_t PushFuncAndArgs(int32_t a, int32_t b, const std::shared_ptr<api::LuaVM> &vm) {
  if (b >= 1) {
    vm->CheckStack(b);
    for (int64_t i = a; i < a + b; i++)
      vm->PushValue(i);
    return b - 1;
  } else {
    FixStack(a, vm);
    return vm->GetTop() - vm->RegisterCount() - 1;
  }
}
void PopResults(int32_t a, int32_t c, const std::shared_ptr<api::LuaVM> &vm) {
  if (c == 1)
    return;
  if (c > 1) {
    for (int64_t i = a + c - 2; i >= a; i--)
      vm->Replace(i);
    return;
  }
  vm->CheckStack(1);
  vm->PushInteger(int64_t(a));
}
}

void Closure(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, bx]=i.ABx();
  ++a;

  vm->LoadProto(bx);
  vm->Replace(a);
}

void Call(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();

  ++a;
  auto n_args = PushFuncAndArgs(a, b, vm);
  vm->Call(n_args, c - 1);
  PopResults(a, c, vm);
}

void Return(Instruction ins, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] =ins.ABC();
  ++a;

  if (b == 1)
    return;
  if (b > 1) {
    vm->CheckStack(b - 1);
    for (int64_t i = a; i <= a + b - 2; i++)
      vm->PushValue(i);
    return;
  }
  FixStack(a, vm);
}

void VarArg(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] =i.ABC();
  ++a;

  if (b != 1) {
    vm->LoadVarArg(b - 1);
    PopResults(a, b, vm);
  }
}

void TailCall(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] =i.ABC();
  ++a;
  c = 0;

  auto n_args = PushFuncAndArgs(a, b, vm);
  vm->Call(n_args, c - 1);
  PopResults(a, c, vm);
}

void Self(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] =i.ABC();
  ++a;
  ++b;

  vm->Copy(b, a + 1);
  vm->GetRK(c);
  vm->GetTable(b);
  vm->Replace(a);
}

void TForCall(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c]= i.ABC();
  ++a;

  PushFuncAndArgs(a, 3, vm);
  vm->Call(2, c);
  PopResults(a + 3, c + 1, vm);
}

void TForLoop(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, s_bx] =i.AsBx();
  ++a;

  if (!vm->IsNoneOrNil(a + 1)) {
    vm->Copy(a + 1, a);
    vm->AddPC(s_bx);
  }
}
}