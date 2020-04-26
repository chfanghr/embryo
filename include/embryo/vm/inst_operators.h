//
// Created by 方泓睿 on 2020/4/9.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_VM_INST_OPERATORS_H_
#define EMBRYO_INCLUDE_EMBRYO_VM_INST_OPERATORS_H_

#include <embryo/api/lua_vm.h>
#include <embryo/vm/instruction.h>

namespace embryo::vm {
void Add(Instruction, const std::shared_ptr<api::LuaVM> &);

void Sub(Instruction, const std::shared_ptr<api::LuaVM> &);

void Mul(Instruction, const std::shared_ptr<api::LuaVM> &);

void Mod(Instruction, const std::shared_ptr<api::LuaVM> &);

void Pow(Instruction, const std::shared_ptr<api::LuaVM> &);

void Div(Instruction, const std::shared_ptr<api::LuaVM> &);

void IDiv(Instruction, const std::shared_ptr<api::LuaVM> &);

void BAnd(Instruction, const std::shared_ptr<api::LuaVM> &);

void BOr(Instruction, const std::shared_ptr<api::LuaVM> &);

void BXor(Instruction, const std::shared_ptr<api::LuaVM> &);

void ShL(Instruction, const std::shared_ptr<api::LuaVM> &);

void ShR(Instruction, const std::shared_ptr<api::LuaVM> &);

void UnM(Instruction, const std::shared_ptr<api::LuaVM> &);

void BNot(Instruction, const std::shared_ptr<api::LuaVM> &);

void Len(Instruction, const std::shared_ptr<api::LuaVM> &);

void Concat(Instruction, const std::shared_ptr<api::LuaVM> &);

void EQ(Instruction, const std::shared_ptr<api::LuaVM> &);

void LT(Instruction, const std::shared_ptr<api::LuaVM> &);

void LE(Instruction, const std::shared_ptr<api::LuaVM> &);

void Not(Instruction, const std::shared_ptr<api::LuaVM> &);

void TestSet(Instruction, const std::shared_ptr<api::LuaVM> &);

void Test(Instruction, const std::shared_ptr<api::LuaVM> &);

void ForPrep(Instruction, const std::shared_ptr<api::LuaVM> &);

void ForLoop(Instruction, const std::shared_ptr<api::LuaVM> &);
}
#endif //EMBRYO_INCLUDE_EMBRYO_VM_INST_OPERATORS_H_
