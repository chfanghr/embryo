//
// Created by 方泓睿 on 2020/4/15.
//

#include <stdexcept>
#include <utility>

#include <embryo/utils/inspection.h>
#include <embryo/compiler/code_gen/func_info.h>
#include <embryo/vm/instruction.h>
#include <embryo/vm/opcodes.h>
#include <embryo/utils/fpb.h>

#include <fmt/format.h>

namespace embryo::code_gen {
std::map<lexer::Token, vm::Op> kArithAndBitwiseBinOps{ // NOLINT(cert-err58-cpp)
    {lexer::Token::kOpAdd, vm::Op::kAdd},
    {lexer::kOpSub, vm::Op::kSub},
    {lexer::Token::kOpMul, vm::Op::kMul},
    {lexer::Token::kOpMod, vm::Op::kMod},
    {lexer::Token::kOpPow, vm::Op::kPow},
    {lexer::Token::kOpDiv, vm::Op::kDiv},
    {lexer::Token::kOpIDiv, vm::Op::kIDiv},
    {lexer::Token::kOpBAnd, vm::Op::kBAnd},
    {lexer::Token::kOpBOr, vm::Op::kBOr},
    {lexer::kOpBXor, vm::Op::kBXor},
    {lexer::Token::kOpShL, vm::Op::kShL},
    {lexer::Token::kOpShR, vm::Op::kShR}
};

int32_t FuncInfo::IndexOfConstant(const LuaConstant &key) {
  auto iter = constants_.find(key);
  if (iter != constants_.end())
    return iter->second;
  auto idx = constants_.size();
  constants_[key] = idx;
  return idx;
}

int32_t FuncInfo::AllocReg() {
  ++used_regs_;
  if (used_regs_ >= 255)
    Error(MSG_WITH_FUNC_NAME("function or expression desired too much registers"));
  if (used_regs_ > max_regs_)
    max_regs_ = used_regs_;
  return used_regs_ - 1;
}

void FuncInfo::FreeReg() {
  if (used_regs_ <= 0)
    Error(MSG_WITH_FUNC_NAME("no more allocated registers to be freed"));
  --used_regs_;
}

int32_t FuncInfo::AllocRegs(int32_t n) {
  if (n <= 0)
    Error(MSG_WITH_FUNC_NAME("desired number of registers less than zero"));
  for (size_t i = 0; i < n; i++)
    AllocReg();
  return used_regs_ - n;
}

void FuncInfo::FreeRegs(size_t n) {
  for (size_t i = 0; i < n; i++)
    FreeReg();
}

void FuncInfo::Error(const std::string &msg) const {
  throw std::runtime_error(fmt::format("{}: {}", THIS_TYPE, msg));
}

void FuncInfo::EnterScope(bool breakable) {
  ++scope_lv_;
  if (breakable)
    breaks_.emplace_back(std::vector<int32_t>());
  else
    breaks_.emplace_back();
}

int32_t FuncInfo::AddLocVar(const std::string &name, int32_t start_pc) {
  auto new_var = std::make_shared<LocVarInfo>(loc_names_[name],
                                              name,
                                              scope_lv_,
                                              AllocReg(),
                                              false,
                                              start_pc);
  loc_vars_.push_back(new_var);
  loc_names_[name] = new_var;
  return new_var->slot;
}

LocVarInfo::LocVarInfo(std::shared_ptr<LocVarInfo> prev,
                       std::string name,
                       int32_t scope_lv,
                       int32_t slot,
                       bool captured,
                       int32_t start_pc)
    : prev(std::move(prev)),
      name(std::move(name)),
      scope_lv(scope_lv),
      slot(slot),
      captured(captured),
      start_pc_(start_pc) {}

int32_t FuncInfo::SlotOfLocVar(const std::string &name) {
  auto iter = loc_names_.find(name);
  if (iter != loc_names_.end())
    return iter->second->slot;
  return -1;
}

void FuncInfo::ExitScope(int32_t end_pc) {
  std::optional<std::vector<int32_t>> pending_break_jmps{};
  if (!breaks_.empty()) {
    pending_break_jmps = breaks_[breaks_.size() - 1];
    breaks_.pop_back();
  }
  auto a = GetJmpArgA();
  if (pending_break_jmps)
    for (auto pc:pending_break_jmps.value()) {
      auto s_bx = PC() - pc;
      uint32_t i =
          (s_bx + vm::kMaxArgSBx) << 14 | a << 6 | static_cast<int32_t>(vm::Op::kJmp); // NOLINT(hicpp-signed-bitwise)
      insts_[pc] = i;
    }
  --scope_lv_;
  auto loc_names = loc_names_;
  for (const auto &[k, loc_var]:loc_names)
    if (loc_var->scope_lv > scope_lv_) {
      loc_var->end_pc_ = end_pc;
      RemoveLocVar(loc_var);
    }
}

void FuncInfo::RemoveLocVar(const std::shared_ptr<LocVarInfo> &loc_var) {
  FreeReg();
  if (!loc_var->prev)
    loc_names_.erase(loc_var->name);
  else if (loc_var->prev->scope_lv == scope_lv_)
    RemoveLocVar(loc_var->prev);
  else
    loc_names_[loc_var->name] = loc_var->prev;
}

void FuncInfo::AddBreakJmp(int32_t pc) {
  for (auto i = scope_lv_; i >= 0; i--) {
    if (breaks_[i]) {
      breaks_[i].value().push_back(pc);
      return;
    }
  }
  Error(MSG_WITH_FUNC_NAME("<break> at line ? not inside a loop"));
}

UpValInfo::UpValInfo(int32_t loc_var_slot, int32_t up_val_index, int32_t index)
    : loc_var_slot(loc_var_slot), up_val_index(up_val_index), index(index) {}

int32_t FuncInfo::IndexOfUpVal(const std::string &name) {
  {
    auto iter = up_values_.find(name);
    if (iter != up_values_.end())
      return iter->second.index;
  }
  if (parent_) {
    {
      auto iter = parent_->loc_names_.find(name);
      if (iter != parent_->loc_names_.end()) {
        auto idx = up_values_.size();
        auto loc_var = iter->second;
        up_values_[name] = UpValInfo{loc_var->slot, -1, static_cast<int32_t>(idx)};
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedValue"
        loc_var->captured = true;
#pragma clang diagnostic pop
        return idx;
      }
    }
    {
      auto uv_idx = parent_->IndexOfUpVal(name);
      if (uv_idx >= 0) {
        auto idx = up_values_.size();
        up_values_[name] = UpValInfo(-1, uv_idx, idx);
        return idx;
      }
    }
  }

  return -1;
}

void FuncInfo::EmitABC(int32_t line, vm::Op opcode, int32_t a, int32_t b, int32_t c) {
  insts_.push_back(b << 23 | c << 14 | a << 6 | static_cast<int32_t>(opcode)); // NOLINT(hicpp-signed-bitwise)
  line_nums_.push_back(line);
}

void FuncInfo::EmitABx(int32_t line, vm::Op opcode, int32_t a, int32_t bx) {
  insts_.push_back(bx << 14 | a << 6 | static_cast<int32_t>(opcode)); // NOLINT(hicpp-signed-bitwise)
  line_nums_.push_back(line);
}

void FuncInfo::EmitAsBx(int32_t line, vm::Op opcode, int32_t as, int32_t bx) {
  insts_.push_back(
      (bx + vm::kMaxArgSBx) << 14 | as << 6 | static_cast<int32_t>(opcode)); // NOLINT(hicpp-signed-bitwise)
  line_nums_.push_back(line);
}

void FuncInfo::EmitAx(int32_t line, vm::Op opcode, int32_t ax) {
  insts_.push_back(ax << 6 | static_cast<int32_t>(opcode)); // NOLINT(hicpp-signed-bitwise)
  line_nums_.push_back(line);
}

int32_t FuncInfo::PC() {
  return insts_.size() - 1;
}

void FuncInfo::FixSBx(int32_t pc, int32_t s_bx) {
  auto i = insts_[pc];
  i = i << 18 >> 18; // NOLINT(hicpp-signed-bitwise)
  i = i | uint32_t(s_bx + vm::kMaxArgSBx) << 24; // NOLINT(hicpp-signed-bitwise)
  insts_[pc] = i;
}

FuncInfo::FuncInfo(std::shared_ptr<FuncInfo> parent,
                   const std::shared_ptr<ast::FuncDefExp> &fd) :
    parent_(std::move(parent)),
    num_params_(fd->par_list_.size()),
    is_var_arg(fd->is_var_arg_),
    line_(fd->line_),
    last_line_(fd->last_line_) {}

int32_t FuncInfo::GetJmpArgA() {
  bool has_captured_loc_vars = false;
  auto min_slot_of_loc_vars = max_regs_;

  for (const auto &[name, loc_var] : loc_names_) {
    if (loc_var->scope_lv == scope_lv_) {
      for (auto v = loc_var; v && v->scope_lv == scope_lv_; v = v->prev) {
        if (v->captured)
          has_captured_loc_vars = true;
        if (v->slot < min_slot_of_loc_vars && v->name[0] != '(')
          min_slot_of_loc_vars = v->slot;
      }
    }
  }

  if (has_captured_loc_vars)
    return min_slot_of_loc_vars + 1;
  return 0;
}

void FuncInfo::EmitMove(int32_t line, int32_t a, int32_t b) {
  EmitABC(line, vm::Op::kMove, a, b, 0);
}

void FuncInfo::EmitLoadNil(int32_t line, int32_t a, int32_t n) {
  EmitABC(line, vm::Op::kLoadNil, a, n - 1, 0);
}

void FuncInfo::EmitLoadBool(int32_t line, int32_t a, int32_t b, int32_t c) {
  EmitABC(line, vm::Op::kLoadBool, a, b, c);
}

void FuncInfo::EmitLoadK(int32_t line, int32_t a, const LuaConstant &k) {
  auto idx = IndexOfConstant(k);
  if (idx < (1 << 18)) // NOLINT(hicpp-signed-bitwise)
    EmitABx(line, vm::Op::kLoadK, a, idx);
  else {
    EmitABx(line, vm::Op::kLoadK, a, 0);
    EmitAx(line, vm::Op::kExtraArg, idx);
  }
}

void FuncInfo::EmitVarArg(int32_t line, int32_t a, int32_t n) {
  EmitABC(line, vm::Op::kVarArg, a, n + 1, 0);
}

void FuncInfo::EmitClosure(int32_t line, int32_t a, int32_t bx) {
  EmitABx(line, vm::Op::kClosure, a, bx);
}

void FuncInfo::EmitNewTable(int32_t line, int32_t a, int32_t n_arr, int32_t n_rec) {
  EmitABC(line, vm::Op::kNewTable, a, utils::IntToFb(n_arr), utils::IntToFb(n_rec));
}

void FuncInfo::EmitSetList(int32_t line, int32_t a, int32_t b, int32_t c) {
  EmitABC(line, vm::Op::kSetList, a, b, c);
}

void FuncInfo::EmitGetTable(int32_t line, int32_t a, int32_t b, int32_t c) {
  EmitABC(line, vm::Op::kGetTable, a, b, c);
}

void FuncInfo::EmitSetTable(int32_t line, int32_t a, int32_t b, int32_t c) {
  EmitABC(line, vm::Op::kSetTable, a, b, c);
}

void FuncInfo::EmitGetUpVal(int32_t line, int32_t a, int32_t b) {
  EmitABC(line, vm::Op::kGetUpVal, a, b, 0);
}

void FuncInfo::EmitSetUpVal(int32_t line, int32_t a, int32_t b) {
  EmitABC(line, vm::Op::kSetUpVal, a, b, 0);
}

void FuncInfo::EmitGetTabUp(int32_t line, int32_t a, int32_t b, int32_t c) {
  EmitABC(line, vm::Op::kGetTabUp, a, b, c);
}

void FuncInfo::EmitSetTabUp(int32_t line, int32_t a, int32_t b, int32_t c) {
  EmitABC(line, vm::Op::kSetTabUp, a, b, c);
}

void FuncInfo::EmitCall(int32_t line, int32_t a, int32_t n_args, int32_t n_ret) {
  EmitABC(line, vm::Op::kCall, a, n_args + 1, n_ret + 1);
}

void FuncInfo::EmitTailCall(int32_t line, int32_t a, int32_t n_args) {
  EmitABC(line, vm::Op::kTailCall, a, n_args + 1, 0);
}

void FuncInfo::EmitReturn(int32_t line, int32_t a, int32_t n) {
  EmitABC(line, vm::Op::kReturn, a, n + 1, 0);
}

void FuncInfo::EmitSelf(int32_t line, int32_t a, int32_t b, int32_t c) {
  EmitABC(line, vm::Op::kSelf, a, b, c);
}

int32_t FuncInfo::EmitJmp(int32_t line, int32_t a, int32_t s_bx) {
  EmitAsBx(line, vm::Op::kJmp, a, s_bx);
  return insts_.size() - 1;
}

void FuncInfo::EmitUnaryOp(int32_t line, lexer::Token op, int32_t a, int32_t b) {
  switch (op) {
    case lexer::Token::kOpNot:EmitABC(line, vm::Op::kNot, a, b, 0);
      return;
    case lexer::kOpBNot:EmitABC(line, vm::Op::kBNot, a, b, 0);
      return;
    case lexer::Token::kOpLen:EmitABC(line, vm::Op::kLen, a, b, 0);
      return;
    case lexer::kOpUnM:EmitABC(line, vm::Op::kUnM, a, b, 0);
      return;
    default:;
  }
}

void FuncInfo::EmitBinaryOp(int32_t line, lexer::Token op, int32_t a, int32_t b, int32_t c) {
  auto iter = kArithAndBitwiseBinOps.find(op);
  if (iter != kArithAndBitwiseBinOps.end()) {
    EmitABC(line, iter->second, a, b, c);
    return;
  }

  switch (op) {
    case lexer::Token::kOpEQ:EmitABC(line, vm::Op::kEQ, 1, b, c);
      break;
    case lexer::Token::kOpNE:EmitABC(line, vm::Op::kEQ, 0, b, c);
      break;
    case lexer::Token::kOpLT:EmitABC(line, vm::Op::kLT, 1, b, c);
      break;
    case lexer::Token::kOpGT:EmitABC(line, vm::Op::kLT, 1, c, b);
      break;
    case lexer::Token::kOpLE:EmitABC(line, vm::Op::kLE, 1, b, c);
      break;
    case lexer::Token::kOpGE:EmitABC(line, vm::Op::kLE, 1, c, b);
      break;
    default:;
  }

  EmitJmp(line, 0, 1);
  EmitLoadBool(line, a, 0, 1);
  EmitLoadBool(line, a, 1, 0);
}

void FuncInfo::EmitTest(int32_t line, int32_t a, int32_t c) {
  EmitABC(line, vm::Op::kTest, a, 0, c);
}

void FuncInfo::EmitTestSet(int32_t line, int32_t a, int32_t b, int32_t c) {
  EmitABC(line, vm::Op::kTestSet, a, b, c);
}

int32_t FuncInfo::EmitForPrep(int32_t line, int32_t a, int32_t s_bx) {
  EmitAsBx(line, vm::Op::kForPrep, a, s_bx);
  return insts_.size() - 1;
}

int32_t FuncInfo::EmitForLoop(int32_t line, int32_t a, int32_t s_bx) {
  EmitAsBx(line, vm::Op::kForLoop, a, s_bx);
  return insts_.size() - 1;
}

void FuncInfo::EmitTForCall(int32_t line, int32_t a, int32_t c) {
  EmitABC(line, vm::Op::kTForCall, a, 0, c);
}

void FuncInfo::EmitTForLoop(int32_t line, int32_t a, int32_t s_bx) {
  EmitAsBx(line, vm::Op::kTForLoop, a, s_bx);
}

void FuncInfo::CloseOpenUpVals(int32_t line) {
  auto a = GetJmpArgA();
  if (a > 0)
    EmitJmp(line, a, 0);
}

void FuncInfo::FixEndPC(const std::string &name, int32_t delta) {
  for (int64_t i = loc_vars_.size() - 1; i >= 0; --i) {
    auto loc_var = loc_vars_[i];
    if (loc_var->name == name) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedValue"
      loc_var->end_pc_ += delta;
#pragma clang diagnostic pop
      return;
    }
  }
}
}