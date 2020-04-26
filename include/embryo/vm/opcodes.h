//
// Created by 方泓睿 on 2020/4/7.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_VM_OPCODES_H_
#define EMBRYO_INCLUDE_EMBRYO_VM_OPCODES_H_

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <memory>

#include <embryo/utils/stringer.h>
#include <embryo/utils/types.h>
#include <embryo/vm/constants.h>
#include <embryo/vm/instruction.h>
#include <embryo/api/lua_vm.h>

namespace embryo::vm {
using InstAction =
std::function< void(Instruction,
const std::shared_ptr<api::LuaVM> &)>;

struct Opcode : utils::Stringer {
  Byte test_flag{}; // operator is a test(next must be a jmp)
  Byte set_a_flag{}; // instruction set register A
  OpArg arg_b_mode{}; // B arg mode
  OpArg arg_c_mode{}; // C arg mode
  OpMode op_mode{}; // op mode
  std::string name{};
  InstAction action_;

  Opcode() = default;
  Opcode(Byte test_flag, Byte set_a_flag,
         OpArg arg_b_mode, OpArg arg_c_mode, OpMode op_mode,
         std::string name, InstAction action)
      : test_flag(test_flag),
        set_a_flag(set_a_flag),
        arg_b_mode(arg_b_mode),
        arg_c_mode(arg_c_mode),
        op_mode(op_mode),
        name(std::move(name)),
        action_(std::move(action)) {}

  [[nodiscard]] std::string String() const override;
};

extern const std::vector<Opcode> kOpcodes;
}
#endif //EMBRYO_INCLUDE_EMBRYO_VM_OPCODES_H_
