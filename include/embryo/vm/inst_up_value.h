//
// Created by 方泓睿 on 2020/4/11.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_VM_INST_UP_VALUE_H_
#define EMBRYO_INCLUDE_EMBRYO_VM_INST_UP_VALUE_H_

#include <embryo/vm/instruction.h>
#include <embryo/api/lua_vm.h>

namespace embryo::vm {
void GetTabUp(Instruction, const std::shared_ptr<api::LuaVM> &);

void GetUpVal(Instruction, const std::shared_ptr<api::LuaVM> &);

void SetUpVal(Instruction, const std::shared_ptr<api::LuaVM> &);

void SetTabUp(Instruction, const std::shared_ptr<api::LuaVM> &);
}
#endif //EMBRYO_INCLUDE_EMBRYO_VM_INST_UP_VALUE_H_
