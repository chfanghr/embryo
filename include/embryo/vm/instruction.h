//
// Created by 方泓睿 on 2020/4/7.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_VM_INSTRUCTION_H_
#define EMBRYO_INCLUDE_EMBRYO_VM_INSTRUCTION_H_

#include <tuple>
#include <string>
#include <memory>

#include <embryo/api/lua_vm.h>
#include <embryo/vm/constants.h>

namespace embryo::vm {
const uint32_t kMaxArgBx = (1U << 18U) - 1; // 262143
const uint32_t kMaxArgSBx = kMaxArgBx >> 1U; // 131071

class Instruction {
 public:
  uint32_t instruction_{};

 public:
  Instruction() = default;
  explicit Instruction(uint32_t instruction) : instruction_(instruction) {}
  Instruction &operator=(const Instruction &i) = default;

  bool operator==(const Instruction &i) const {
    return i.instruction_ == instruction_;
  }

  operator uint32_t() const; // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

  [[nodiscard]] Op Opcode() const;

  [[nodiscard]] std::tuple<int32_t, int32_t, int32_t> ABC() const; // a,b,c

  [[nodiscard]] std::tuple<int32_t, int32_t> ABx() const; // a,bx

  [[nodiscard]] std::tuple<int32_t, int32_t> AsBx() const; // a,sbx

  [[nodiscard]] int32_t Ax() const;

  [[nodiscard]] std::string OpName() const;

  [[nodiscard]] OpMode Mode() const;

  [[nodiscard]] OpArg BMode() const;

  [[nodiscard]] OpArg CMode() const;

  void PrettyPrint() const;

  void Execute(const std::shared_ptr<api::LuaVM> &vm);
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_VM_INSTRUCTION_H_
