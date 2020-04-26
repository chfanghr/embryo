//
// Created by 方泓睿 on 2020/4/16.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_EXP_H_
#define EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_EXP_H_

#include <tuple>

#include <embryo/compiler/ast/exp.h>
#include <embryo/compiler/code_gen/func_info.h>

namespace embryo::code_gen {
enum class ArgKind : int32_t {
  kConst = 1,
  kReg = 2,
  kUpVal = 4,
  kRK = kReg | kConst, // NOLINT(hicpp-signed-bitwise)
  kRU = kReg | kUpVal, // NOLINT(hicpp-signed-bitwise)
  kRUK = kReg | kUpVal | kConst // NOLINT(hicpp-signed-bitwise)
};

void CGExp(const std::shared_ptr<FuncInfo> &fi,
           const std::shared_ptr<ast::Exp> &exp,
           int32_t a, int32_t n);

std::tuple<int32_t, ArgKind> ExpToOpArg(const std::shared_ptr<FuncInfo> &fi,
                                        const std::shared_ptr<ast::Exp> &exp,
                                        ArgKind args_kinds);

void CGTailCallExp(const std::shared_ptr<FuncInfo> &fi,
                   const std::shared_ptr<ast::FuncCallExp> &exp,
                   int32_t a);

void CGFuncDefExp(const std::shared_ptr<FuncInfo> &fi,
                  const std::shared_ptr<ast::FuncDefExp> &exp,
                  int32_t a);

void CGFuncCallExp(const std::shared_ptr<FuncInfo> &fi,
                   const std::shared_ptr<ast::FuncCallExp> &exp,
                   int32_t a,
                   int32_t n);

void CGVarArgExp(const std::shared_ptr<FuncInfo> &fi,
                 const std::shared_ptr<ast::VarArgExp> &exp,
                 int32_t a,
                 int32_t n);

void CGTableConstructorExp(const std::shared_ptr<FuncInfo> &fi,
                           const std::shared_ptr<ast::TableConstructorExp> &exp,
                           int32_t a);

void CGUnOpExp(const std::shared_ptr<FuncInfo> &fi,
               const std::shared_ptr<ast::UnOpExp> &exp,
               int32_t a);

void CGBinOpExp(const std::shared_ptr<FuncInfo> &fi,
                const std::shared_ptr<ast::BinOpExp> &exp,
                int32_t a);

void CGConcatExp(const std::shared_ptr<FuncInfo> &fi,
                 const std::shared_ptr<ast::ConcatExp> &exp,
                 int32_t a);

void CGNameExp(const std::shared_ptr<FuncInfo> &fi,
               const std::shared_ptr<ast::NameExp> &exp,
               int32_t a);

void CGTableAccessExp(const std::shared_ptr<FuncInfo> &fi,
                      const std::shared_ptr<ast::TableAccessExp> &exp,
                      int32_t a);
}
#endif //EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_EXP_H_
