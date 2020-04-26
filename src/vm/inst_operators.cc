//
// Created by 方泓睿 on 2020/4/9.
//

#include <embryo/vm/inst_operators.h>

namespace embryo::vm {
namespace {
void UnaryArith(Instruction i, const std::shared_ptr<api::LuaVM> &vm, api::ArithOp op) {
  auto[a, b, c]=i.ABC();
  ++a;
  ++b;

  vm->PushValue(b);
  vm->Arith(op);
  vm->Replace(a);
}

void BinaryArith(Instruction i, const std::shared_ptr<api::LuaVM> &vm, api::ArithOp op) {
  auto[a, b, c]=i.ABC();
  ++a;

  vm->GetRK(b);
  vm->GetRK(c);
  vm->Arith(op);
  vm->Replace(a);
}

void Compare(Instruction i, const std::shared_ptr<api::LuaVM> &vm, api::CompareOp op) {
  auto[a, b, c]=i.ABC();

  vm->GetRK(b);
  vm->GetRK(c);
  if (vm->Compare(-2, -1, op) != (a != 0))
    vm->AddPC(1);

  vm->Pop(2);
}
}

void Add(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kAdd); }

void Sub(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kSub); }

void Mul(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kMul); }

void Mod(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kMod); }

void Pow(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kPow); }

void Div(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kDiv); }

void IDiv(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kIDiv); }

void BAnd(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kBAnd); }

void BOr(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kBOr); }

void BXor(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kBXor); }

void ShL(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kShL); }

void ShR(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { BinaryArith(i, vm, api::ArithOp::kShR); }

void UnM(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { UnaryArith(i, vm, api::ArithOp::kUnM); }

void BNot(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { UnaryArith(i, vm, api::ArithOp::kBNot); }

void Len(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;
  ++b;

  vm->Len(b);
  vm->Replace(a);
}

void Concat(Instruction ins, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c]=ins.ABC();
  ++a;
  ++b;
  ++c;

  auto n = c - b + 1;
  vm->CheckStack(n);
  for (int64_t i = b; i <= c; i++)
    vm->PushValue(i);

  vm->Concat(n);
  vm->Replace(a);
}

void EQ(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { Compare(i, vm, api::CompareOp::kEQ); }

void LT(Instruction i, const std::shared_ptr<api::LuaVM> &vm) { Compare(i, vm, api::CompareOp::kLT); }

void LE(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {{ Compare(i, vm, api::CompareOp::kLE); }}

void Not(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;
  ++b;

  vm->PushBoolean(!vm->ToBoolean(b));
  vm->Replace(a);
}

void TestSet(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;
  ++b;

  if (vm->ToBoolean(b) == (c != 0))
    vm->Copy(b, a);
  else
    vm->AddPC(1);
}

void Test(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, b, c] = i.ABC();
  ++a;

  if (vm->ToBoolean(a) != (c != 0))
    vm->AddPC(1);
}

void ForPrep(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, s_bx]=i.AsBx();
  ++a;

  vm->PushValue(a);
  vm->PushValue(a + 2);
  vm->Arith(api::ArithOp::kSub);
  vm->Replace(a);
  vm->AddPC(s_bx);
}

void ForLoop(Instruction i, const std::shared_ptr<api::LuaVM> &vm) {
  auto[a, s_bx]=i.AsBx();
  ++a;

  vm->PushValue(a + 2);
  vm->PushValue(a);
  vm->Arith(api::ArithOp::kAdd);
  vm->Replace(a);

  bool is_positive_step = vm->ToNumber(a + 2) >= 0;
  if ((is_positive_step && vm->Compare(a, a + 1, api::CompareOp::kLE)) ||
      (!is_positive_step && vm->Compare(a + 1, a, api::CompareOp::kLE))) {
    vm->AddPC(s_bx);
    vm->Copy(a, a + 3);
  }
}
}