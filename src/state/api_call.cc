//
// Created by 方泓睿 on 2020/4/10.
//

#include <embryo/bin_chunk/reader.h>
#include <embryo/bin_chunk/closure.h>
#include <embryo/state/lua_state.h>
#include <embryo/utils/inspection.h>
#include <embryo/state/lua_table.h>
#include <embryo/compiler/compiler.h>

#include <fmt/format.h>

namespace embryo::state {
api::Status LuaState::Load(const std::string &file) {
  return Load(compiler::Compile(file));
}

api::Status LuaState::Load(const std::shared_ptr<bin_chunk::Prototype> &proto) {
  auto closure = std::make_shared<LuaClosure>(proto);
  stack_->
      Push(std::dynamic_pointer_cast<LuaAnyType>(closure));
  if (!proto->up_values.empty()) {
    auto env = registry_->Get(kLuaRIdxGlobals);
    closure->up_values[0] = std::make_shared<UpValue>(env);
  }
  return api::Status::kOk; //  TODO
}

api::Status LuaState::Load(
    const std::vector<Byte> &chunk,
    std::string chunk_name, std::string mode
) {
  auto proto = bin_chunk::IsBinChunk(chunk) ? bin_chunk::Undump(chunk) : compiler::Compile(chunk, chunk_name);
  return Load(proto);
}

void LuaState::Call(int32_t n_args, int32_t n_results) {
  auto val = stack_->Get(-(n_args + 1));
  auto[c, ok] =TryCast<LuaClosurePtr>(val);

  if (!ok) {
    auto mf = state::GetMetaField(val, "__call", shared_from_this());
    if (mf) {
      std::tie(c, ok) = TryCast<LuaClosurePtr>(mf);
      if (ok) {
        stack_->Push(val);
        Insert(-(n_args + 2));
        n_args++;
      }
    }
  }

  if (ok) {
    if (state::IsNativeFunction(c))
      CallNativeClosure(n_args, n_results, c);
    else
      CallLuaClosure(n_args, n_results, c);
    return;
  }

  Error(MSG_WITH_FUNC_NAME("given LuaValue is not callable"));
}

void LuaState::CallLuaClosure(
    int32_t n_args, int32_t n_results, const embryo::state::LuaClosurePtr &c) {
  int64_t n_regs = c->proto_->max_stack_size;
  int64_t n_params = c->proto_->num_params;
  bool is_var_arg = c->proto_->is_var_arg == 1;

  auto new_stack = std::make_shared<LuaStack>(n_regs + kLuaMinStack, this); // TODO
  new_stack->closure_ = c;

  auto func_and_args = stack_->PopN(n_args + 1);
  new_stack->PushN(std::vector<LuaValue>(
      func_and_args.begin() + 1, func_and_args.end()), n_params);
  new_stack->top_ = n_regs;
  if (n_args > n_params && is_var_arg)
    new_stack->var_args_ = std::vector<LuaValue>(
        func_and_args.begin() + n_params + 1,
        func_and_args.end());

  PushLuaStack(new_stack);
  RunLuaClosure();
  PopLuaStack();

  if (n_results) {
    auto results = new_stack->PopN(new_stack->top_ - n_regs);
    stack_->Check(results.size());
    stack_->PushN(results, n_results);
  }
}

void LuaState::RunLuaClosure() {
  while (true) {
    auto inst = vm::Instruction(Fetch());
//    std::string line = "-";
//    if (!stack_->closure_->proto_->line_info.empty())
//      line = fmt::format("{}", stack_->closure_->proto_->line_info[PC()]);
//    fmt::print("\t{}\t[{:>3}]\t{:<9} \t", PC(), line, inst.OpName());
//    inst.PrettyPrint();
//    fmt::print("\n");
    inst.Execute(std::dynamic_pointer_cast<api::LuaVM>(shared_from_this()));
    if (inst.Opcode() == vm::Op::kReturn)
      break;
  }
}

void LuaState::CallNativeClosure(int32_t n_args, int32_t n_results,
                                 const LuaClosurePtr &c) {
  auto new_stack = std::make_shared<LuaStack>(n_args + kLuaMinStack, this);
  new_stack->closure_ = c;

  auto args = stack_->PopN(n_args);
  new_stack->PushN(args, n_args);
  stack_->Pop();

  PushLuaStack(new_stack);
  auto r = c->native_function_(shared_from_this());
  PopLuaStack();

  if (n_results != 0) {
    auto results = new_stack->PopN(r);
    stack_->Check(results.size());
    stack_->PushN(results, n_results);
  }
}

api::Status LuaState::PCall(int32_t n_args, int32_t n_results, int32_t msgh) {
  auto caller = stack_;

  try {
    Call(n_args, n_results);
    return api::Status::kOk;
  } catch (const std::exception &err) {
    while (stack_ != caller)
      PopLuaStack();
    stack_->Push(std::string(err.what()));
    return api::Status::kErrRun;
  }
}
}
