//
// Created by 方泓睿 on 2020/4/4.
//

#include <embryo/compiler/parser/parser.h>

namespace embryo::parser {
std::shared_ptr<ast::Block> Parser::ParseBlock() {
  auto res = std::make_shared<ast::Block>();

  res->chunk_name_ = lexer_.ChunkName();
  res->stats_ = ParseStats();
  res->ret_exps_ = ParseRetExps();
  res->last_line_ = lexer_.Line();

  return res;
}

bool Parser::IsReturnOrBlockEnd(const lexer::Lexer::ScanRes &scan_res) {
  static const auto tokens = {lexer::Token::kKwReturn,
                              lexer::Token::kEOF,
                              lexer::Token::kKwEnd,
                              lexer::Token::kKwElse,
                              lexer::Token::kKwElseIf,
                              lexer::Token::kKwUntil,
  };

  for (const auto &token:tokens)
    if (token == std::get<1>(scan_res))
      return true;

  return false;
}

std::vector<std::shared_ptr<ast::Stat>> Parser::ParseStats() {
  std::vector<std::shared_ptr<ast::Stat>> stats{};

  while (!IsReturnOrBlockEnd(lexer_.LookAhead())) {
    auto stat = ParseStat();

    auto empty_stat = std::dynamic_pointer_cast<ast::EmptyStat>(stat);
    if (!empty_stat)
      stats.push_back(stat);
  }

  return stats;
}

std::vector<std::shared_ptr<ast::Exp>> Parser::ParseExpList() {
  std::vector<std::shared_ptr<ast::Exp>> exps{};
  exps.push_back(ParseExp());
  while (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepComma) { // skip ,
    lexer_.NextToken();
    exps.push_back(ParseExp());
  }
  return exps;
}

std::vector<std::shared_ptr<ast::Exp>> Parser::ParseRetExps() {
  if (std::get<1>(lexer_.LookAhead()) != lexer::Token::kKwReturn)
    return {};

  lexer_.NextToken(); // skip return
  switch (std::get<1>(lexer_.LookAhead())) {
    case lexer::Token::kEOF:
    case lexer::Token::kKwEnd:
    case lexer::Token::kKwElse:
    case lexer::Token::kKwElseIf:
    case lexer::Token::kKwUntil:return {};
    case lexer::Token::kSepSemi: // skip ;
      lexer_.NextToken();
      return {};
    default:;
  }

  auto exps = ParseExpList();
  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepSemi) // skip ;
    lexer_.NextToken();
  return exps;
}
}