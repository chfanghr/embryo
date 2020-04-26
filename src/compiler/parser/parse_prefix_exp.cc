//
// Created by 方泓睿 on 2020/4/5.
//

#include <embryo/compiler/parser/parser.h>

namespace embryo::parser {
std::shared_ptr<ast::Exp> Parser::ParsePrefixExp() {
  std::shared_ptr<ast::Exp> exp{};
  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kIdentifier) {
    auto[line, tok, name]=lexer_.NextToken();
    exp = std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::NameExp>(line, name));
  } else exp = ParseParenExp();

  return FinishPrefixExp(exp);
}

std::shared_ptr<ast::Exp> Parser::ParseParenExp() {
  lexer_.NextTokenOfKind(lexer::Token::kSepLParen); // (
  auto exp = ParseExp();
  lexer_.NextTokenOfKind(lexer::Token::kSepRParen); // )

  if (std::dynamic_pointer_cast<ast::VarArgExp>(exp) ||
      std::dynamic_pointer_cast<ast::FuncCallExp>(exp) ||
      std::dynamic_pointer_cast<ast::NameExp>(exp) ||
      std::dynamic_pointer_cast<ast::TableAccessExp>(exp))
    return std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::ParensExp>(exp));

  return exp;
}

std::shared_ptr<ast::Exp> Parser::FinishPrefixExp(std::shared_ptr<ast::Exp> exp) {
  while (true) {
    switch (std::get<1>(lexer_.LookAhead())) {
      case lexer::Token::kSepLBracket: {
        lexer_.NextToken(); // [
        auto key_exp = ParseExp();
        lexer_.NextTokenOfKind(lexer::Token::kSepRBracket); //]
        exp = std::dynamic_pointer_cast<ast::Exp>(
            std::make_shared<ast::TableAccessExp>(lexer_.Line(), exp, key_exp));
        break;
      }
      case lexer::Token::kSepDot: {// prefixexp . Name
        lexer_.NextToken();
        auto[line, tok, name]=lexer_.NextTokenOfKind(lexer::Token::kIdentifier);
        auto key_exp = std::dynamic_pointer_cast<ast::Exp>(
            std::make_shared<ast::StringExp>(line, name));
        exp = std::dynamic_pointer_cast<ast::Exp>(
            std::make_shared<ast::TableAccessExp>(line, exp, key_exp));
        break;
      }
      case lexer::Token::kSepColon:
      case lexer::Token::kSepLParen:
      case lexer::Token::kSepLCurly:
      case lexer::Token::kString:exp = FinishFuncCallExp(exp);
        break;
      default:return exp;
    }
  }
}

std::shared_ptr<ast::Exp> Parser::FinishFuncCallExp(std::shared_ptr<ast::Exp> exp) {
  auto name_exp = ParseNameExp();
  auto line = lexer_.Line();
  auto args = ParseArgs();
  auto last_line = lexer_.Line();
  return std::dynamic_pointer_cast<ast::Exp>(
      std::make_shared<ast::FuncCallExp>(line, last_line, exp, name_exp, args));
}

std::shared_ptr<ast::StringExp> Parser::ParseNameExp() {
  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepColon) {
    lexer_.NextToken();
    auto[line, tok, lit] =lexer_.NextTokenOfKind(lexer::Token::kIdentifier);
    return std::make_shared<ast::StringExp>(line, lit);
  }
  return nullptr;
}

std::vector<std::shared_ptr<ast::Exp>> Parser::ParseArgs() {
  std::vector<std::shared_ptr<ast::Exp>> args{};
  switch (std::get<1>(lexer_.LookAhead())) {
    case lexer::Token::kSepLParen: // ( [explist] )
      lexer_.NextToken(); // (
      if (std::get<1>(lexer_.LookAhead()) != lexer::Token::kSepRParen)
        args = ParseExpList();
      lexer_.NextTokenOfKind(lexer::Token::kSepRParen); // )
      break;
    case lexer::Token::kSepLCurly: // { fieldlist }
      args = {ParseTableConstructorExp()};
      break;
    default:auto[line, tok, str]=lexer_.NextTokenOfKind(lexer::Token::kString);
      args = {std::make_shared<ast::StringExp>(line, str)};
  }

  return args;
}
}