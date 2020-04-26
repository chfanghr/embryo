//
// Created by 方泓睿 on 2020/4/4.
//

#include <utility>

#include <embryo/compiler/parser/parser.h>

namespace embryo::parser {
std::shared_ptr<ast::EmptyStat> Parser::ParseEmptyStat() {
  lexer_.NextTokenOfKind(lexer::Token::kSepSemi); // ;
  return std::make_shared<ast::EmptyStat>();
}

std::shared_ptr<ast::BreakStat> Parser::ParseBreakStat() {
  lexer_.NextTokenOfKind(lexer::Token::kKwBreak);
  return std::make_shared<ast::BreakStat>(lexer_.Line());
}

std::shared_ptr<ast::LabelStat> Parser::ParseLabelStat() {
  lexer_.NextTokenOfKind(lexer::Token::kSepLabel); // ::
  std::string name{};
  std::tie(std::ignore, std::ignore, name) = lexer_.NextTokenOfKind(lexer::Token::kIdentifier);
  lexer_.NextTokenOfKind(lexer::Token::kSepLabel); // ::
  return std::make_shared<ast::LabelStat>(name);
}

std::shared_ptr<ast::GotoStat> Parser::ParseGotoStat() {
  lexer_.NextTokenOfKind(lexer::Token::kKwGoto);
  std::string name{};
  std::tie(std::ignore, std::ignore, name) = lexer_.NextTokenOfKind(lexer::Token::kIdentifier);
  return std::make_shared<ast::GotoStat>(name);
}

std::shared_ptr<ast::DoStat> Parser::ParseDoStat() {
  lexer_.NextTokenOfKind(lexer::Token::kKwDo); // do
  auto block = ParseBlock(); // block
  lexer_.NextTokenOfKind(lexer::Token::kKwEnd); // end
  return std::make_shared<ast::DoStat>(block);
}

std::shared_ptr<ast::WhileStat> Parser::ParseWhileStat() {
  lexer_.NextTokenOfKind(lexer::Token::kKwWhile); // while
  auto exp = ParseExp(); // exp
  lexer_.NextTokenOfKind(lexer::Token::kKwDo); // while
  auto block = ParseBlock(); // block
  lexer_.NextTokenOfKind(lexer::Token::kKwEnd); // end
  return std::make_shared<ast::WhileStat>(exp, block);
}

std::shared_ptr<ast::RepeatStat> Parser::ParseRepeatStat() {
  lexer_.NextTokenOfKind(lexer::Token::kKwRepeat); // repeat
  auto block = ParseBlock(); // block
  lexer_.NextTokenOfKind(lexer::Token::kKwUntil); // repeat
  auto exp = ParseExp(); // exp
  return std::make_shared<ast::RepeatStat>(exp, block);
}

std::shared_ptr<ast::IfStat> Parser::ParseIfStat() {
  std::vector<std::shared_ptr<ast::Exp>> exps{};
  std::vector<std::shared_ptr<ast::Block>> blocks{};

  lexer_.NextTokenOfKind(lexer::Token::kKwIf); // if
  exps.push_back(ParseExp()); // exp
  lexer_.NextTokenOfKind(lexer::Token::kKwThen); // then
  blocks.push_back(ParseBlock());

  while (std::get<1>(lexer_.LookAhead()) == lexer::Token::kKwElseIf) {
    lexer_.NextToken(); // elseif
    exps.push_back(ParseExp()); // exp
    lexer_.NextTokenOfKind(lexer::Token::kKwThen); // then
    blocks.push_back(ParseBlock());
  }

  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kKwElse) {
    lexer_.NextToken(); // else
    exps.push_back(std::make_shared<ast::TrueExp>(lexer_.Line()));
    blocks.push_back(ParseBlock()); // block
  }

  lexer_.NextTokenOfKind(lexer::Token::kKwEnd); // end

  return std::make_shared<ast::IfStat>(exps, blocks);
}

std::shared_ptr<ast::Stat> Parser::ParseForStat() {
  auto line_of_for = std::get<0>(lexer_.NextTokenOfKind(lexer::Token::kKwFor)); // for
  auto name = std::get<2>(lexer_.NextTokenOfKind(lexer::Token::kIdentifier)); // name

  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kOpAssign) // =
    return std::dynamic_pointer_cast<ast::Stat>(FinishForNumStat(line_of_for, name));

  return std::dynamic_pointer_cast<ast::Stat>(FinishInNumStat(name));
}

std::shared_ptr<ast::ForNumStat> Parser::FinishForNumStat(size_t line_of_for, const std::string &var_name) {
  lexer_.NextTokenOfKind(lexer::Token::kOpAssign); // =
  auto init_exp = ParseExp(); // exp
  lexer_.NextTokenOfKind(lexer::Token::kSepComma); // ,
  auto limit_exp = ParseExp(); // exp

  std::shared_ptr<ast::Exp> step_exp{};

  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepComma) {
    lexer_.NextToken(); // ,
    step_exp = ParseExp();
  } else
    step_exp = std::dynamic_pointer_cast<ast::Exp>(
        std::make_shared<ast::IntegerExp>(lexer_.Line(), 1));

  auto line_of_do = std::get<0>(lexer_.NextTokenOfKind(lexer::Token::kKwDo)); // do
  auto block = ParseBlock(); // block
  lexer_.NextTokenOfKind(lexer::Token::kKwEnd); // end

  return std::make_shared<ast::ForNumStat>
      (line_of_for, line_of_do, var_name, init_exp, limit_exp, step_exp, block);
}

std::shared_ptr<ast::ForInStat> Parser::FinishInNumStat(const std::string &name_0) {
  auto name_list = FinishNameList(name_0); // namelist
  lexer_.NextTokenOfKind(lexer::Token::kKwIn); // in
  auto exp_list = ParseExpList(); // explist
  auto line_of_do = std::get<0>(lexer_.NextTokenOfKind(lexer::Token::kKwDo)); // do
  auto block = ParseBlock(); // block
  lexer_.NextTokenOfKind(lexer::Token::kKwEnd); // end

  return std::make_shared<ast::ForInStat>(line_of_do, name_list, exp_list, block);
}

std::vector<std::string> Parser::FinishNameList(std::string name_0) {
  std::vector<std::string> names{std::move(name_0)};
  while (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepComma) {
    lexer_.NextToken(); // ,
    names.push_back(std::get<2>(lexer_.NextTokenOfKind(lexer::Token::kIdentifier)));
  }
  return names;
}

std::shared_ptr<ast::Stat> Parser::ParseLocalAssignOrFuncDefStat() {
  lexer_.NextTokenOfKind(lexer::Token::kKwLocal);
  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kKwFunction)
    return std::dynamic_pointer_cast<ast::Stat>(FinishLocalFuncDefStat());

  return std::dynamic_pointer_cast<ast::Stat>(FinishLocalVarDeclStat());
}

std::shared_ptr<ast::LocalFuncDefStat> Parser::FinishLocalFuncDefStat() {
  lexer_.NextTokenOfKind(lexer::Token::kKwFunction); // function
  auto name = std::get<2>(lexer_.NextTokenOfKind(lexer::Token::kIdentifier)); // name
  auto fd_exp = ParseFuncDefExp(); // funcbody

  return std::make_shared<ast::LocalFuncDefStat>(name, fd_exp);
}

std::shared_ptr<ast::LocalVarDeclStat> Parser::FinishLocalVarDeclStat() {
  auto name_0 = std::get<2>(lexer_.NextTokenOfKind(lexer::Token::kIdentifier)); // name
  auto name_list = FinishNameList(name_0);

  std::vector<std::shared_ptr<ast::Exp>> exp_list{};

  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kOpAssign) {
    lexer_.NextToken(); // =
    exp_list = ParseExpList();
  }

  auto last_line = lexer_.Line();
  return std::make_shared<ast::LocalVarDeclStat>(last_line, name_list, exp_list);
}

std::shared_ptr<ast::Stat> Parser::ParseAssignOrFuncCallStat() {
  auto prefix_exp = ParsePrefixExp();
  auto fc = std::dynamic_pointer_cast<ast::FuncCallExp>(prefix_exp);
  if (fc)
    return std::dynamic_pointer_cast<ast::Stat>(prefix_exp);

  return std::dynamic_pointer_cast<ast::Stat>(ParseAssignStat(prefix_exp));
}

std::shared_ptr<ast::AssignStat> Parser::ParseAssignStat(std::shared_ptr<ast::Exp> &prefix_exp) {
  auto var_list = FinishVarList(prefix_exp); // varlist
  lexer_.NextTokenOfKind(lexer::Token::kOpAssign); // =
  auto exp_list = ParseExpList(); // explist
  auto last_line = lexer_.Line();

  return std::make_shared<ast::AssignStat>(last_line, var_list, exp_list);
}

std::vector<std::shared_ptr<ast::Exp>> Parser::FinishVarList(const std::shared_ptr<ast::Exp> &var_0) {
  std::vector<std::shared_ptr<ast::Exp>> vars{CheckVar(var_0)};

  while (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepComma) {
    lexer_.NextToken(); // ,
    auto exp = ParsePrefixExp();
    vars.push_back(CheckVar(var_0));
  }

  return vars;
}

std::shared_ptr<ast::Exp> Parser::CheckVar(std::shared_ptr<ast::Exp> exp) {
  if (std::dynamic_pointer_cast<ast::NameExp>(exp) ||
      std::dynamic_pointer_cast<ast::TableAccessExp>(exp))
    return exp;
  Error("unexpected expression type");
}

std::shared_ptr<ast::AssignStat> Parser::ParseFuncDefStat() {
  lexer_.NextTokenOfKind(lexer::Token::kKwFunction); // function
  auto[fn_exp, has_colon] = ParseFuncName();
  auto fd_exp = ParseFuncDefExp();

  if (has_colon) {
    fd_exp->par_list_.emplace_back("");
    fd_exp->par_list_.insert(fd_exp->par_list_.begin(), "self");
  }

  return std::make_shared<ast::AssignStat>(fd_exp->line_,
                                           std::vector<std::shared_ptr<ast::Exp>>{fn_exp},
                                           std::vector<std::shared_ptr<ast::Exp>>{fd_exp});
}

std::tuple<std::shared_ptr<ast::Exp>, bool> Parser::ParseFuncName() {
  bool has_colon = false;
  auto[line, tok, name]= lexer_.NextTokenOfKind(lexer::Token::kIdentifier);
  auto exp = std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::NameExp>(line, name));

  while (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepDot) {
    lexer_.NextToken();
    auto[line, tok, name]= lexer_.NextTokenOfKind(lexer::Token::kIdentifier);
    auto idx = std::make_shared<ast::StringExp>(line, name);
    exp = std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::TableAccessExp>(line, exp, idx));
  }

  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepColon) {
    lexer_.NextToken();
    auto[line, tok, name]= lexer_.NextTokenOfKind(lexer::Token::kIdentifier);
    auto idx = std::make_shared<ast::StringExp>(line, name);
    exp = std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::TableAccessExp>(line, exp, idx));
    has_colon = true;
  }

  return {exp, has_colon};
}

std::shared_ptr<ast::Stat> Parser::ParseStat() {
  switch (std::get<1>(lexer_.LookAhead())) {
    case lexer::Token::kSepSemi: return std::dynamic_pointer_cast<ast::Stat>(ParseEmptyStat());
    case lexer::Token::kKwBreak: return std::dynamic_pointer_cast<ast::Stat>(ParseBreakStat());
    case lexer::Token::kSepLabel:return std::dynamic_pointer_cast<ast::Stat>(ParseLabelStat());
    case lexer::Token::kKwGoto: return std::dynamic_pointer_cast<ast::Stat>(ParseGotoStat());
    case lexer::Token::kKwWhile: return std::dynamic_pointer_cast<ast::Stat>(ParseWhileStat());
    case lexer::Token::kKwRepeat: return std::dynamic_pointer_cast<ast::Stat>(ParseRepeatStat());
    case lexer::Token::kKwIf: return std::dynamic_pointer_cast<ast::Stat>(ParseIfStat());
    case lexer::Token::kKwFor: return std::dynamic_pointer_cast<ast::Stat>(ParseForStat());
    case lexer::Token::kKwFunction: return std::dynamic_pointer_cast<ast::Stat>(ParseFuncDefStat());
    case lexer::Token::kKwLocal: return std::dynamic_pointer_cast<ast::Stat>(ParseLocalAssignOrFuncDefStat());
    default: return std::dynamic_pointer_cast<ast::Stat>(ParseAssignOrFuncCallStat());
  }
}
}