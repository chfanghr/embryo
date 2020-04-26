//
// Created by 方泓睿 on 2020/4/5.
//

#include <embryo/utils/numbers.h>
#include <embryo/compiler/parser/parser.h>
#include <embryo/compiler/parser/optimizers.h>

namespace embryo::parser {
std::shared_ptr<ast::Exp> Parser::ParseExp() {
  return ParseExp12();
}

// exp ::=  nil | false | true | Numeral | LiteralString | ‘...’ | functiondef |
//	 prefixexp | tableconstructor | exp binop exp | unop exp


// exp   ::= exp12
// exp12 ::= exp11 {or exp11}
// exp11 ::= exp10 {and exp10}
// exp10 ::= exp9 {(‘<’ | ‘>’ | ‘<=’ | ‘>=’ | ‘~=’ | ‘==’) exp9}
// exp9  ::= exp8 {‘|’ exp8}
// exp8  ::= exp7 {‘~’ exp7}
// exp7  ::= exp6 {‘&’ exp6}
// exp6  ::= exp5 {(‘<<’ | ‘>>’) exp5}
// exp5  ::= exp4 {‘..’ exp4}
// exp4  ::= exp3 {(‘+’ | ‘-’) exp3}
// exp3  ::= exp2 {(‘*’ | ‘/’ | ‘//’ | ‘%’) exp2}
// exp2  ::= {(‘not’ | ‘#’ | ‘-’ | ‘~’)} exp1
// exp1  ::= exp0 {‘^’ exp2}
// exp0  ::= nil | false | true | Numeral | LiteralString
// 		| ‘...’ | functiondef | prefixexp | tableconstructor

std::shared_ptr<ast::Exp> Parser::ParseExpCommon(const std::vector<lexer::Token> &accept_tokens,
                                                 const std::function<std::shared_ptr<ast::Exp>()> &next_level,
                                                 const std::function<std::shared_ptr<ast::Exp>(const std::shared_ptr<ast::Exp> &)> &optimizer) {
  auto exp = next_level();

  auto accept_token = [&](lexer::Token tok) {
    for (const auto token:accept_tokens)
      if (tok == token)
        return true;
    return false;
  };

  while (accept_token(std::get<1>(lexer_.LookAhead()))) {
    auto[line, tok, name]= lexer_.NextToken();
    exp = std::dynamic_pointer_cast<ast::Exp>
        (std::make_shared<ast::BinOpExp>(line, tok, exp, next_level()));
  }

  if (optimize_ && optimizer) {
    try {
      exp = optimizer(exp);
    } catch (const std::runtime_error &error) {
      Error(fmt::format("optimizer error: {}", error.what()));
    }
  }

  return exp;
}

// x or y
std::shared_ptr<ast::Exp> Parser::ParseExp12() {
  return ParseExpCommon({lexer::Token::kOpOr},
                        std::bind(&Parser::ParseExp11, this),
                        LogicalOrOptimizer);
}

// x and y
std::shared_ptr<ast::Exp> Parser::ParseExp11() {
  return ParseExpCommon({lexer::Token::kOpAnd},
                        std::bind(&Parser::ParseExp10, this),
                        LogicalAndOptimizer);
}

// compare
std::shared_ptr<ast::Exp> Parser::ParseExp10() {
  return ParseExpCommon({lexer::Token::kOpLT,
                         lexer::Token::kOpGT,
                         lexer::Token::kOpLE,
                         lexer::Token::kOpGE,
                         lexer::Token::kOpEQ,
                         lexer::Token::kOpNE},
                        std::bind(&Parser::ParseExp9, this));
}

// x | y
std::shared_ptr<ast::Exp> Parser::ParseExp9() {
  return ParseExpCommon({lexer::Token::kOpBOr},
                        std::bind(&Parser::ParseExp8, this),
                        BitWiseBinOpOptimizer);
}

// x ~ y
std::shared_ptr<ast::Exp> Parser::ParseExp8() {
  return ParseExpCommon({lexer::kOpBXor},
                        std::bind(&Parser::ParseExp7, this),
                        BitWiseBinOpOptimizer);
}

// x & y
std::shared_ptr<ast::Exp> Parser::ParseExp7() {
  return ParseExpCommon({lexer::Token::kOpBAnd},
                        std::bind(&Parser::ParseExp6, this),
                        BitWiseBinOpOptimizer);
}

// shift
std::shared_ptr<ast::Exp> Parser::ParseExp6() {
  return ParseExpCommon({lexer::Token::kOpShL, lexer::Token::kOpShR},
                        std::bind(&Parser::ParseExp5, this),
                        BitWiseBinOpOptimizer);
}

// ..
std::shared_ptr<ast::Exp> Parser::ParseExp5() {
  auto exp = ParseExp4();
  if (std::get<1>(lexer_.LookAhead()) != lexer::Token::kOpConcat)
    return exp;

  size_t line = 0;
  std::vector<std::shared_ptr<ast::Exp>> exps{};

  while (std::get<1>(lexer_.LookAhead()) == lexer::Token::kOpConcat) {
    std::tie(line, std::ignore, std::ignore) = lexer_.NextToken();
    exps.push_back(ParseExp4());
  }

  return std::make_shared<ast::ConcatExp>(line, exps);
}

// x +/- y
std::shared_ptr<ast::Exp> Parser::ParseExp4() {
  return ParseExpCommon({lexer::Token::kOpAdd, lexer::kOpSub},
                        std::bind(&Parser::ParseExp3, this),
                        ArithBinaryOpOptimizer);
}

// *,%,/,//
std::shared_ptr<ast::Exp> Parser::ParseExp3() {
  return ParseExpCommon({lexer::Token::kOpMul,
                         lexer::Token::kOpMod,
                         lexer::Token::kOpDiv,
                         lexer::Token::kOpIDiv},
                        std::bind(&Parser::ParseExp2, this),
                        ArithBinaryOpOptimizer);
}

// unary
std::shared_ptr<ast::Exp> Parser::ParseExp2() {
  static const auto accept_tokens = {
      lexer::kOpUnM,
      lexer::kOpBNot,
      lexer::Token::kOpLen,
      lexer::Token::kOpNot
  };

  auto accept_token = [&](lexer::Token tok) {
    for (const auto token:accept_tokens)
      if (tok == token)
        return true;
    return false;
  };

  if (accept_token(std::get<1>(lexer_.LookAhead()))) {
    auto[line, tok, name]= lexer_.NextToken();
    auto exp = std::make_shared<ast::UnOpExp>(line, tok, ParseExp2());
    try {
      return optimize_ ? UnaryOpOptimizer(exp) : exp;
    } catch (const std::runtime_error &error) {
      Error(fmt::format("optimizer error: {}", error.what()));
    }
  }

  return ParseExp1();
}

// ^
std::shared_ptr<ast::Exp> Parser::ParseExp1() {
  auto exp = ParseExp0();
  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kOpPow) {
    auto[line, tok, name]= lexer_.NextToken();
    exp = std::make_shared<ast::UnOpExp>(line, tok, ParseExp2());
  }
  try {
    return optimize_ ? PowOptimizer(exp) : exp;
  } catch (const std::runtime_error &error) {
    Error(fmt::format("optimizer error: {}", error.what()));
  }
}

std::shared_ptr<ast::Exp> Parser::ParseExp0() {
  switch (std::get<1>(lexer_.LookAhead())) {
    case lexer::Token::kVarArg: // ...
    {
      auto[line, tok, name]= lexer_.NextToken();
      return std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::VarArgExp>(line));
    }
    case lexer::Token::kKwNil: // nil
    {
      auto[line, tok, name]= lexer_.NextToken();
      return std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::NilExp>(line));
    }
    case lexer::Token::kKwTrue: // true
    {
      auto[line, tok, name]= lexer_.NextToken();
      return std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::TrueExp>(line));
    }
    case lexer::Token::kKwFalse: // false
    {
      auto[line, tok, name]= lexer_.NextToken();
      return std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::FalseExp>(line));
    }
    case lexer::Token::kString: // string literal
    {
      auto[line, tok, lit]= lexer_.NextToken();
      return std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::StringExp>(line, lit));
    }
    case lexer::Token::kNumber: // number literal
      return ParseNumberExp();
    case lexer::Token::kSepLCurly: // table constructor
      return ParseTableConstructorExp();
    case lexer::Token::kKwFunction: // function definition
      lexer_.NextToken();
      return ParseFuncDefExp();
    default:return ParsePrefixExp();
  }
}

std::shared_ptr<ast::Exp> Parser::ParseNumberExp() {
  auto[line, tok, lit]= lexer_.NextToken();

  auto[i, ok_i]= utils::ParseInt(lit);
  if (ok_i)
    return std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::IntegerExp>(line, i));

  auto[f, ok_f]=utils::ParseFloat(lit);
  if (ok_f)
    return std::dynamic_pointer_cast<ast::Exp>(std::make_shared<ast::FloatExp>(line, f));

  Error(fmt::format("not a number: {}", lit));
}

std::shared_ptr<ast::FuncDefExp> Parser::ParseFuncDefExp() {
  auto line = lexer_.Line();
  lexer_.NextTokenOfKind(lexer::Token::kSepLParen); // (
  auto[par_list, is_var_arg]= ParseParList(); // [parlist]
  lexer_.NextTokenOfKind(lexer::Token::kSepRParen); // )
  auto block = ParseBlock();
  auto[last_line, tok, lit]=lexer_.NextTokenOfKind(lexer::Token::kKwEnd);

  return std::make_shared<ast::FuncDefExp>(line, last_line, par_list, is_var_arg, block);
}

std::tuple<std::vector<std::string>, bool> Parser::ParseParList() {
  switch (std::get<1>(lexer_.LookAhead())) {
    case lexer::Token::kSepRParen:return {{}, false};
    case lexer::Token::kVarArg:lexer_.NextToken();
      return {{}, true};
    default:;
  }

  std::vector<std::string> names{};
  bool is_var_arg = false;

  auto[line, tok, ident]=lexer_.NextTokenOfKind(lexer::Token::kIdentifier);
  names.push_back(ident);

  while (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepComma) {
    lexer_.NextToken();
    if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kIdentifier) {
      auto[line, tok, ident]=lexer_.NextToken();
      names.push_back(ident);
    } else {
      lexer_.NextTokenOfKind(lexer::Token::kVarArg);
      is_var_arg = true;
      break;
    }
  }

  return {names, is_var_arg};
}

std::shared_ptr<ast::TableConstructorExp> Parser::ParseTableConstructorExp() {
  auto line = lexer_.Line();
  lexer_.NextTokenOfKind(lexer::Token::kSepLCurly); // {
  auto[key_exps, val_exps]= ParseFieldList();
  lexer_.NextTokenOfKind(lexer::Token::kSepRCurly); // }
  auto last_line = lexer_.Line();
  return std::make_shared<ast::TableConstructorExp>(line, last_line, key_exps, val_exps);
}

bool Parser::IsFieldSep(lexer::Token tok) {
  return tok == lexer::Token::kSepComma || tok == lexer::Token::kSepSemi; // , or ;
}

std::tuple<std::vector<std::shared_ptr<ast::Exp>>,
           std::vector<std::shared_ptr<ast::Exp>>> Parser::ParseFieldList() {
  std::vector<std::shared_ptr<ast::Exp>> ks{}, vs{};
  if (std::get<1>(lexer_.LookAhead()) != lexer::Token::kSepRCurly) {
    auto[k, v] =ParseField();
    ks.push_back(k);
    vs.push_back(v);

    while (IsFieldSep(std::get<1>(lexer_.LookAhead()))) {
      lexer_.NextToken();
      if (std::get<1>(lexer_.LookAhead()) != lexer::Token::kSepRCurly) {
        auto[k, v] =ParseField();
        ks.push_back(k);
        vs.push_back(v);
      } else break;
    }
  }
  return {ks, vs};
}

std::tuple<std::shared_ptr<ast::Exp>, std::shared_ptr<ast::Exp>> Parser::ParseField() {
  if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kSepLBracket) {
    lexer_.NextToken(); // [
    auto k = ParseExp();
    lexer_.NextTokenOfKind(lexer::Token::kSepRBracket); // ]
    lexer_.NextTokenOfKind(lexer::Token::kOpAssign); // =
    auto v = ParseExp();
    return {k, v};
  }

  auto exp = ParseExp();
  if (std::dynamic_pointer_cast<ast::NameExp>(exp)) {
    if (std::get<1>(lexer_.LookAhead()) == lexer::Token::kOpAssign) {
      auto name_exp = std::dynamic_pointer_cast<ast::NameExp>(exp);
      lexer_.NextToken();
      auto k = std::dynamic_pointer_cast<ast::Exp>(
          std::make_shared<ast::StringExp>(name_exp->line_, name_exp->name_));
      auto v = ParseExp();
      return {k, v};
    }
  }

  return {nullptr, exp};
}
}