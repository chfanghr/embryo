//
// Created by 方泓睿 on 2020/4/9.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_VM_INST_MISC_H_
#define EMBRYO_INCLUDE_EMBRYO_VM_INST_MISC_H_

#include <embryo/api/lua_vm.h>
#include <embryo/vm/instruction.h>

namespace embryo::vm {
void Move(Instruction, const std::shared_ptr<api::LuaVM> &);

void Jmp(Instruction, const std::shared_ptr<api::LuaVM> &);
}

#endif //EMBRYO_INCLUDE_EMBRYO_VM_INST_MISC_H_
