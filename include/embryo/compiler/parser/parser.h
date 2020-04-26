//
// Created by 方泓睿 on 2020/4/4.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_PARSER_PARSER_H_
#define EMBRYO_INCLUDE_EMBRYO_PARSER_PARSER_H_

#include <functional>

#include <embryo/compiler/ast/common.h>
#include <embryo/compiler/ast/block.h>
#include <embryo/compiler/ast/exp.h>
#include <embryo/compiler/ast/stat.h>
#include <embryo/compiler/lexer/lexer.h>

namespace embryo::parser {
template<typename T>
std::shared_ptr<ast::Block> Parse(const T &chunk, const std::string &chunk_name);

template<>
std::shared_ptr<ast::Block> Parse<std::string>(const std::string &chunk, const std::string &chunk_name);

template<>
std::shared_ptr<ast::Block> Parse<std::vector<Byte>>(const std::vector<Byte> &chunk, const std::string &chunk_name);

class Parser final {
 private:
  lexer::Lexer lexer_;

  const bool optimize_;
 public:
  Parser(const std::string &chunk, const std::string &chunk_name, bool optimize = true);

  std::shared_ptr<ast::Block> Parse();

 private:
  std::shared_ptr<ast::Block> ParseBlock();

  std::vector<std::shared_ptr<ast::Exp>> ParseRetExps();

  std::vector<std::shared_ptr<ast::Stat>> ParseStats();

  std::shared_ptr<ast::Stat> ParseStat();

  std::shared_ptr<ast::Exp> ParseExp();

  std::vector<std::shared_ptr<ast::Exp>> ParseExpList();

 private:
  [[noreturn]] void Error(const std::string &msg);

 private:
  static bool IsReturnOrBlockEnd(const lexer::Lexer::ScanRes &scan_res);

  static bool IsFieldSep(lexer::Token tok);
 private:
  std::shared_ptr<ast::EmptyStat> ParseEmptyStat();

  std::shared_ptr<ast::BreakStat> ParseBreakStat();

  std::shared_ptr<ast::LabelStat> ParseLabelStat();

  std::shared_ptr<ast::GotoStat> ParseGotoStat();

  std::shared_ptr<ast::DoStat> ParseDoStat();

  std::shared_ptr<ast::WhileStat> ParseWhileStat();

  std::shared_ptr<ast::RepeatStat> ParseRepeatStat();

  std::shared_ptr<ast::IfStat> ParseIfStat();

  std::shared_ptr<ast::Stat> ParseForStat();

  std::shared_ptr<ast::ForNumStat> FinishForNumStat(size_t line_of_for, const std::string &var_name);

  std::shared_ptr<ast::ForInStat> FinishInNumStat(const std::string &name_0);

  std::vector<std::string> FinishNameList(std::string name_0);

  std::shared_ptr<ast::Stat> ParseLocalAssignOrFuncDefStat();

  std::shared_ptr<ast::LocalFuncDefStat> FinishLocalFuncDefStat();

  std::shared_ptr<ast::LocalVarDeclStat> FinishLocalVarDeclStat();

  std::shared_ptr<ast::Stat> ParseAssignOrFuncCallStat();

  std::shared_ptr<ast::AssignStat> ParseAssignStat(std::shared_ptr<ast::Exp> &prefix_exp);

  std::vector<std::shared_ptr<ast::Exp>> FinishVarList(const std::shared_ptr<ast::Exp> &var_0);

  std::shared_ptr<ast::Exp> CheckVar(std::shared_ptr<ast::Exp>);

  std::shared_ptr<ast::AssignStat> ParseFuncDefStat();

  std::tuple<std::shared_ptr<ast::Exp>, bool> ParseFuncName();

 private:
  std::shared_ptr<ast::Exp> ParseExpCommon(const std::vector<lexer::Token> &accept_tokens,
                                           const std::function<std::shared_ptr<ast::Exp>()> &next_level,
                                           const std::function<std::shared_ptr<ast::Exp>(const std::shared_ptr<ast::Exp> &)> &optimizer = nullptr);

  std::shared_ptr<ast::Exp> ParseExp12();

  std::shared_ptr<ast::Exp> ParseExp11();

  std::shared_ptr<ast::Exp> ParseExp10();

  std::shared_ptr<ast::Exp> ParseExp9();

  std::shared_ptr<ast::Exp> ParseExp8();

  std::shared_ptr<ast::Exp> ParseExp7();

  std::shared_ptr<ast::Exp> ParseExp6();

  std::shared_ptr<ast::Exp> ParseExp5();

  std::shared_ptr<ast::Exp> ParseExp4();

  std::shared_ptr<ast::Exp> ParseExp3();

  std::shared_ptr<ast::Exp> ParseExp2();

  std::shared_ptr<ast::Exp> ParseExp1();

  std::shared_ptr<ast::Exp> ParseExp0();

  std::shared_ptr<ast::FuncDefExp> ParseFuncDefExp();

  std::shared_ptr<ast::Exp> ParsePrefixExp();

  std::shared_ptr<ast::Exp> ParseNumberExp();

  std::shared_ptr<ast::TableConstructorExp> ParseTableConstructorExp();

  std::tuple<std::vector<std::string>, bool> ParseParList();

  std::tuple<std::vector<std::shared_ptr<ast::Exp>>,
             std::vector<std::shared_ptr<ast::Exp>>> ParseFieldList();

  std::tuple<std::shared_ptr<ast::Exp>, std::shared_ptr<ast::Exp>> ParseField();

  std::shared_ptr<ast::Exp> ParseParenExp();

  std::shared_ptr<ast::Exp> FinishPrefixExp(std::shared_ptr<ast::Exp>);

  std::vector<std::shared_ptr<ast::Exp>> ParseArgs();

  std::shared_ptr<ast::Exp> FinishFuncCallExp(std::shared_ptr<ast::Exp>);

  std::shared_ptr<ast::StringExp> ParseNameExp();
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_PARSER_PARSER_H_
