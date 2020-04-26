//
// Created by 方泓睿 on 2020/4/11.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_VM_INST_CALL_H_
#define EMBRYO_INCLUDE_EMBRYO_VM_INST_CALL_H_

#include <embryo/vm/instruction.h>
#include <embryo/api/lua_vm.h>

namespace embryo::vm {
void Closure(Instruction, const std::shared_ptr<api::LuaVM> &);

void Call(Instruction, const std::shared_ptr<api::LuaVM> &);

void Return(Instruction, const std::shared_ptr<api::LuaVM> &);

void VarArg(Instruction, const std::shared_ptr<api::LuaVM> &);

void TailCall(Instruction, const std::shared_ptr<api::LuaVM> &);

void Self(Instruction, const std::shared_ptr<api::LuaVM> &);

void TForCall(Instruction, const std::shared_ptr<api::LuaVM> &);

void TForLoop(Instruction, const std::shared_ptr<api::LuaVM> &);
}
#endif //EMBRYO_INCLUDE_EMBRYO_VM_INST_CALL_H_
