//
// Created by 方泓睿 on 2020/4/15.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_CODEGEN_FUNC_INFO_H_
#define EMBRYO_INCLUDE_EMBRYO_CODEGEN_FUNC_INFO_H_

#include <variant>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <tuple>

#include <embryo/bin_chunk/bin_chunk.h>
#include <embryo/vm/constants.h>
#include <embryo/compiler/ast/exp.h>
#include <embryo/api/constants.h>
#include <embryo/compiler/lexer/token.h>

namespace embryo::code_gen {
using LuaConstant=bin_chunk::LuaConstant;

extern std::map<lexer::Token, vm::Op> kArithAndBitwiseBinOps;

struct LocVarInfo {
  std::shared_ptr<LocVarInfo> prev{};
  std::string name{};
  int32_t scope_lv{};
  int32_t slot{};
  bool captured{};
  int32_t start_pc_{};
  int32_t end_pc_{};

  LocVarInfo() = default;

  LocVarInfo(std::shared_ptr<LocVarInfo> prev,
             std::string name,
             int32_t scope_lv,
             int32_t slot,
             bool captured,
             int32_t start_pc);
};

struct UpValInfo {
  int32_t loc_var_slot{};
  int32_t up_val_index{};
  int32_t index{};

  UpValInfo() = default;

  UpValInfo(int32_t loc_var_slot, int32_t up_val_index, int32_t index);
};

class FuncInfo {
 public:
  std::unordered_map<LuaConstant, int32_t> constants_{};

  int32_t used_regs_{};
  int32_t max_regs_{};

  int32_t scope_lv_{};
  std::vector<std::shared_ptr<LocVarInfo>> loc_vars_{};
  std::map<std::string, std::shared_ptr<LocVarInfo>> loc_names_{};

  std::vector<std::optional<std::vector<int32_t>>> breaks_{};
  std::vector<uint32_t> insts_{};

  std::shared_ptr<FuncInfo> parent_{};
  std::map<std::string, UpValInfo> up_values_{};

  std::vector<std::shared_ptr<FuncInfo>> sub_funcs_{};
  int32_t num_params_{};
  bool is_var_arg{};

  std::vector<uint32_t> line_nums_{};
  int32_t line_{};
  int32_t last_line_{};
 public:
  FuncInfo(std::shared_ptr<FuncInfo> parent, const std::shared_ptr<ast::FuncDefExp> &fd);

  int32_t IndexOfConstant(const LuaConstant &key);

  int32_t AllocReg();

  int32_t AllocRegs(int32_t n);

  void FreeReg();

  void FreeRegs(size_t n);

  void EnterScope(bool breakable);

  int32_t AddLocVar(const std::string &name, int32_t start_pc);

  int32_t SlotOfLocVar(const std::string &name);

  void ExitScope(int32_t end_pc);

  void AddBreakJmp(int32_t pc);

  void RemoveLocVar(const std::shared_ptr<LocVarInfo> &);

  int32_t GetJmpArgA();

  int32_t PC();

  void FixSBx(int32_t pc, int32_t s_bx);

  int32_t IndexOfUpVal(const std::string &name);

  void EmitABC(int32_t line, vm::Op opcode, int32_t a, int32_t b, int32_t c);

  void EmitABx(int32_t line, vm::Op opcode, int32_t a, int32_t bx);

  void EmitAsBx(int32_t line, vm::Op opcode, int32_t as, int32_t bx);

  void EmitAx(int32_t line, vm::Op opcode, int32_t ax);

  void EmitMove(int32_t line, int32_t a, int32_t b);

  void EmitLoadNil(int32_t line, int32_t a, int32_t n);

  void EmitLoadBool(int32_t line, int32_t a, int32_t b, int32_t c);

  void EmitLoadK(int32_t line, int32_t a, const LuaConstant &k);

  void EmitVarArg(int32_t line, int32_t a, int32_t n);

  void EmitClosure(int32_t line, int32_t a, int32_t bx);

  void EmitNewTable(int32_t line, int32_t a, int32_t n_arr, int32_t n_rec);

  void EmitSetList(int32_t line, int32_t a, int32_t b, int32_t c);

  void EmitGetTable(int32_t line, int32_t a, int32_t b, int32_t c);

  void EmitSetTable(int32_t line, int32_t a, int32_t b, int32_t c);

  void EmitGetUpVal(int32_t line, int32_t a, int32_t b);

  void EmitSetUpVal(int32_t line, int32_t a, int32_t b);

  void EmitGetTabUp(int32_t line, int32_t a, int32_t b, int32_t c);

  void EmitSetTabUp(int32_t line, int32_t a, int32_t b, int32_t c);

  void EmitCall(int32_t line, int32_t a, int32_t n_args, int32_t n_ret);

  void EmitTailCall(int32_t line, int32_t a, int32_t n_args);

  void EmitReturn(int32_t line, int32_t a, int32_t n);

  void EmitSelf(int32_t line, int32_t a, int32_t b, int32_t c);

  int32_t EmitJmp(int32_t line, int32_t a, int32_t s_bx);

  void EmitTest(int32_t line, int32_t a, int32_t c);

  void EmitTestSet(int32_t line, int32_t a, int32_t b, int32_t c);

  int32_t EmitForPrep(int32_t line, int32_t a, int32_t s_bx);

  int32_t EmitForLoop(int32_t line, int32_t a, int32_t s_bx);

  void EmitTForCall(int32_t line, int32_t a, int32_t c);

  void EmitTForLoop(int32_t line, int32_t a, int32_t s_bx);

  void EmitUnaryOp(int32_t line, lexer::Token op, int32_t a, int32_t b);

  void EmitBinaryOp(int32_t line, lexer::Token op, int32_t a, int32_t b, int32_t c);

  void CloseOpenUpVals(int32_t line);

  void FixEndPC(const std::string &name, int32_t delta);
 private:
  [[noreturn]] void Error(const std::string &msg) const;
};

}
#endif //EMBRYO_INCLUDE_EMBRYO_CODEGEN_FUNC_INFO_H_
