//
// Created by 方泓睿 on 2020/4/7.
//

#include <embryo/utils/formatters.h>
#include <embryo/bin_chunk/bin_chunk.h>
USING_EMBRYO_FORMATTERS();

namespace embryo::bin_chunk {
std::string Header::String() const {
  return INSPECT_THIS
      .Add(KV(signature))
      .Add(KV(version))
      .Add(KV(format))
      .Add(KV(luac_data))
      .Add(KV(cint_size))
      .Add(KV(size_t_size))
      .Add(KV(instruction_size))
      .Add(KV(lua_integer_size))
      .Add(KV(lua_number_size))
      .Add(KV(luac_int))
      .Add(KV(luac_num))
      .Result();
}

std::string UpValue::String() const {
  return INSPECT_THIS
      .Add(KV(in_stack))
      .Add(KV(idx))
      .Result();
}

std::string LocVar::String() const {
  return INSPECT_THIS
      .Add(KV_AS_STRING(var_name))
      .Add(KV(start_pc))
      .Add(KV(end_pc))
      .Result();
}

std::string Prototype::String() const {
  return INSPECT_THIS
      .Add(KV_AS_STRING(source))
      .Add(KV(line_defined))
      .Add(KV(last_line_defined))
      .Add(KV(num_params))
      .Add(KV(is_var_arg))
      .Add(KV(max_stack_size))
      .Add(KV(code))
      .Add(KV(constants))
      .Add(KV(up_values))
      .Add(KV(protos))
      .Add(KV(line_info))
      .Add(KV(loc_vars))
      .Add(KV_AS_STRINGS(up_value_names))
      .Result();
}
}

