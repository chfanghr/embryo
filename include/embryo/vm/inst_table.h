//
// Created by 方泓睿 on 2020/4/10.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_VM_INST_TABLE_H_
#define EMBRYO_INCLUDE_EMBRYO_VM_INST_TABLE_H_

#include <embryo/api/lua_vm.h>
#include <embryo/vm/instruction.h>

namespace embryo::vm {
const size_t kNFieldPerFlush = 50;

void NewTable(Instruction, const std::shared_ptr<api::LuaVM> &);

void GetTable(Instruction, const std::shared_ptr<api::LuaVM> &);

void SetTable(Instruction, const std::shared_ptr<api::LuaVM> &);

void SetList(Instruction, const std::shared_ptr<api::LuaVM> &);
}
#endif //EMBRYO_INCLUDE_EMBRYO_VM_INST_TABLE_H_
