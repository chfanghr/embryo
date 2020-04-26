//
// Created by 方泓睿 on 2020/4/7.
//

#include <embryo/vm/instruction.h>
#include <embryo/bin_chunk/bin_chunk.h>

namespace embryo::bin_chunk {
void Prototype::PrettyPrint() const {
  PrintHeader();
  PrintCode();
  PrintDetail();
  for (const auto &p:protos)
    p->PrettyPrint();
}

void Prototype::PrintHeader() const {
  std::string func_type = "main";
  if (line_defined > 0)
    func_type = "function";

  std::string var_arg_flag{};

  if (is_var_arg)
    var_arg_flag = "+";

  fmt::print("\n{} <{}:{}:{}> ({} instructions)\n",
             func_type, source, line_defined, last_line_defined, code.size());

  fmt::print("{}{} params, {} slots, {} up_values, ",
             num_params, var_arg_flag, max_stack_size, up_values.size());

  fmt::print("{} locals, {} constants, {} functions\n",
             loc_vars.size(), constants.size(), protos.size());
}

void Prototype::PrintCode() const {
  for (size_t pc = 0; pc < code.size(); pc++) {
    std::string line = "-";
    if (!line_info.empty())
      line = fmt::format("{}", line_info[pc]);
    vm::Instruction i(code[pc]);
    fmt::print("\t{}\t[{}]\t{:<9} \t", pc + 1, line, i.OpName());
    i.PrettyPrint();
    fmt::print("\n");
  }
}

void Prototype::PrintDetail() const {
  fmt::print("constants ({}):\n", constants.size());
  for (size_t i = 0; i < constants.size(); i++)
    fmt::print("\t{}\t{}\n", i + 1, constants[i]);

  fmt::print("locals ({}):\n", loc_vars.size());
  for (size_t i = 0; i < loc_vars.size(); i++)
    fmt::print("\t{}\t{}\t{}\t{}\n",
               i, loc_vars[i].var_name, loc_vars[i].start_pc + 1, loc_vars[i].end_pc + 1);

  fmt::print("up_values ({}):\n", up_values.size());
  for (size_t i = 0; i < up_values.size(); i++)
    fmt::print("\t{}\t{}\t{}\t{}\n",
               i, UpValName(i), up_values[i].in_stack, up_values[i].idx);
}
}