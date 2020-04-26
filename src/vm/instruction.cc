//
// Created by 方泓睿 on 2020/4/7.
//

#include <stdexcept>

#include <fmt/format.h>

#include <embryo/utils/inspection.h>
#include <embryo/vm/instruction.h>
#include <embryo/vm/opcodes.h>

namespace embryo::vm {
// 31       22       13       5    0
//  +-------+^------+-^-----+-^-----
//  |b=9bits |c=9bits |a=8bits|op=6|
//  +-------+^------+-^-----+-^-----
//  |    bx=18bits    |a=8bits|op=6|
//  +-------+^------+-^-----+-^-----
//  |   sbx=18bits    |a=8bits|op=6|
//  +-------+^------+-^-----+-^-----
//  |    ax=26bits            |op=6|
//  +-------+^------+-^-----+-^-----
// 31      23      15       7      0

Instruction::operator uint32_t() const { return instruction_; }

Op Instruction::Opcode() const {
  return static_cast<Op>(instruction_ & uint32_t(0x3F));
}

std::tuple<int32_t, int32_t, int32_t> Instruction::ABC() const {
  return {
      ((int32_t) (((instruction_) >> 6)) & 255), // NOLINT(hicpp-signed-bitwise)
      ((int32_t) ((instruction_) >> 23) & 511), // NOLINT(hicpp-signed-bitwise)
      ((int32_t) ((instruction_) >> 14) & 511), // NOLINT(hicpp-signed-bitwise)
  };
}

std::tuple<int32_t, int32_t> Instruction::ABx() const {
  return {
      ((int32_t) (((instruction_) >> 6)) & 255), // NOLINT(hicpp-signed-bitwise)
      ((int32_t) (((instruction_) >> 14)) & 262143) // NOLINT(hicpp-signed-bitwise)
  };
}

std::tuple<int32_t, int32_t> Instruction::AsBx() const {
  auto[a, bx]=ABx();
  return {a, bx - kMaxArgSBx};
}

int32_t Instruction::Ax() const {
  return ((int32_t) (((instruction_) >> 6)) & 67108863); // NOLINT(hicpp-signed-bitwise)
}

std::string Instruction::OpName() const {
  return kOpcodes[int32_t(Opcode())].name;
}

OpMode Instruction::Mode() const {
  return kOpcodes[int32_t(Opcode())].op_mode;
}

OpArg Instruction::BMode() const {
  return kOpcodes[int32_t(Opcode())].arg_b_mode;
}

OpArg Instruction::CMode() const {
  return kOpcodes[int32_t(Opcode())].arg_c_mode;
}

void Instruction::PrettyPrint() const {
  switch (Mode()) {
    case kABC: {
      auto[a, b, c]=ABC();
      fmt::print("{}", a);
      if (BMode() != OpArg::kN) {
        if (b > 0xFF)
          fmt::print(" {}",
                     -1 - (b & 0xFF)); // NOLINT(hicpp-signed-bitwise)
        else
          fmt::print(" {}", b);
      }
      if (CMode() != OpArg::kN) {
        if (c > 0xFF)
          fmt::print(" {}",
                     -1 - (c & 0xFF)); // NOLINT(hicpp-signed-bitwise)
        else
          fmt::print(" {}", c);
      }
      break;
    }
    case kABx: {
      auto[a, bx]=ABx();
      fmt::print("{}", a);
      if (BMode() == OpArg::kK)
        fmt::print(" {}", -1 - bx);
      else if (BMode() == OpArg::kU)
        fmt::print(" {}", bx);
      break;
    }
    case kAsBx: {
      auto[a, sbx]=AsBx();
      fmt::print("{} {}", a, sbx);
      break;
    }
    case kAx: {
      auto ax = Ax();
      fmt::print(" {}", -1 - ax);
      break;
    }
  }
}

void Instruction::Execute(const std::shared_ptr<api::LuaVM> &vm) {
  auto action = kOpcodes[static_cast<int32_t >(Opcode())].action_;
  if (!action)
    throw std::runtime_error(
        MSG_WITH_FUNC_NAME(fmt::format("no action for {}", OpName())));
  action(*this, vm);
}
}