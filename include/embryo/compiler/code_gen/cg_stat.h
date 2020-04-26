//
// Created by 方泓睿 on 2020/4/16.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_STAT_H_
#define EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_STAT_H_

#include <embryo/compiler/ast/stat.h>
#include <embryo/compiler/code_gen/func_info.h>

namespace embryo::code_gen {
void CGStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::Stat> &stat);

void CGBreakStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::BreakStat> &stat);

void CGLocalFuncDefStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::LocalFuncDefStat> &stat);

void CGFuncCallStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::FuncCallExp> &stat);

void CGDoStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::DoStat> &stat);

void CGRepeatStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::RepeatStat> &stat);

void CGWhileStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::WhileStat> &stat);

void CGIfStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::IfStat> &stat);

void CGForNumStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::ForNumStat> &stat);

void CGForInStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::ForInStat> &stat);

void CGAssignStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::AssignStat> &stat);

void CGLocalVarDeclStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::LocalVarDeclStat> &stat);

void CGLabelStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::LabelStat> &stat);

void CGGotoStat(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::GotoStat> &stat);
}
#endif //EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_STAT_H_
