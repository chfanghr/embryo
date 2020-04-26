//
// Created by 方泓睿 on 2020/4/7.
//

#include <embryo/utils/inspection.h>

#include <embryo/vm/opcodes.h>

namespace embryo::vm {
std::string Opcode::String() const {
  return INSPECT_THIS
      .Add(KV(test_flag))
      .Add(KV(set_a_flag))
      .Add(KV(arg_b_mode))
      .Add(KV(arg_c_mode))
      .Add(KV_AS_STRING(name))
      .Result();
}
}