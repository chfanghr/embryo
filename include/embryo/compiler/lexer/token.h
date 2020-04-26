//
// Created by 方泓睿 on 2020/4/3.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_LEXER_TOKEN_H_
#define EMBRYO_INCLUDE_EMBRYO_LEXER_TOKEN_H_

#include <map>
#include <string>

namespace embryo::lexer {
enum class Token : int16_t {
  kEOF,
  kVarArg, // ...

  kSepSemi, // ;
  kSepComma, // ,
  kSepDot, // .
  kSepColon, // :
  kSepLabel, // ::
  kSepLParen, // (
  kSepRParen, // )
  kSepLBracket, // [
  kSepRBracket, // ]
  kSepLCurly, // {
  kSepRCurly, // }

  kOpAssign, // =
  kOpMinus, // - (sub or unm)
  kOpWave, // ~ (bNot pr bXor)
  kOpAdd, // +
  kOpMul, // *
  kOpDiv, // /
  kOpIDiv, // //
  kOpPow, // ^
  kOpMod, // %
  kOpBAnd, // &
  kOpBOr, // |
  kOpShR, // >>
  kOpShL, // <<
  kOpConcat, // ..
  kOpLT, // <
  kOpLE, // <=
  kOpGT, // >
  kOpGE, // >=
  kOpEQ, // ==
  kOpNE, // ~=
  kOpLen, // #
  kOpAnd, // and
  kOpOr, // or
  kOpNot, // not

  kKwBreak, // break
  kKwDo, // do
  kKwElse, // else
  kKwElseIf, // elseif
  kKwEnd, // end
  kKwFalse, // false
  kKwFor, // for
  kKwFunction, // function
  kKwGoto, // goto
  kKwIf, // if
  kKwIn, // in
  kKwLocal, // local
  kKwNil, // nil
  kKwRepeat, // repeat
  kKwReturn, // return
  kKwThen, // then
  kKwTrue, // true
  kKwUntil, // until
  kKwWhile, // while

  kIdentifier, // identifier
  kNumber, // number literal
  kString, // string literal
};

const std::map<Token, std::string> kTokenNames{ // NOLINT(cert-err58-cpp)
    {Token::kEOF, "Token::kEOF"},
    {Token::kVarArg, "Token::kVarArg"},
    {Token::kSepSemi, "Token::kSepSemi"},
    {Token::kSepComma, "Token::kSepComma"},
    {Token::kSepDot, "Token::kSepDot"},
    {Token::kSepColon, "Token::kSepColon"},
    {Token::kSepLabel, "Token::kSepLabel"},
    {Token::kSepLParen, "Token::kSepLParen"},
    {Token::kSepRParen, "Token::kSepRParen"},
    {Token::kSepLBracket, "Token::kSepLBracket"},
    {Token::kSepRBracket, "Token::kSepRBracket"},
    {Token::kSepLCurly, "Token::kSepLCurly"},
    {Token::kSepRCurly, "Token::kSepRCurly"},
    {Token::kOpAssign, "Token::kOpAssign"},
    {Token::kOpMinus, "Token::kOpMinus"},
    {Token::kOpWave, "Token::kOpWave"},
    {Token::kOpAdd, "Token::kOpAdd"},
    {Token::kOpMul, "Token::kOpMul"},
    {Token::kOpDiv, "Token::kOpDiv"},
    {Token::kOpIDiv, "Token::kOpIDiv"},
    {Token::kOpPow, "Token::kOpPow"},
    {Token::kOpMod, "Token::kOpMod"},
    {Token::kOpBAnd, "Token::kOpBAnd"},
    {Token::kOpBOr, "Token::kOpBOr"},
    {Token::kOpShR, "Token::kOpShR"},
    {Token::kOpShL, "Token::kOpShL"},
    {Token::kOpConcat, "Token::kOpConcat"},
    {Token::kOpLT, "Token::kOpLT"},
    {Token::kOpLE, "Token::kOpLE"},
    {Token::kOpGT, "Token::kOpGT"},
    {Token::kOpGE, "Token::kOpGE"},
    {Token::kOpEQ, "Token::kOpEQ"},
    {Token::kOpNE, "Token::kOpNE"},
    {Token::kOpLen, "Token::kOpLen"},
    {Token::kOpAnd, "Token::kOpAnd"},
    {Token::kOpOr, "Token::kOpOr"},
    {Token::kOpNot, "Token::kOpNot"},
    {Token::kKwBreak, "Token::kKwBreak"},
    {Token::kKwDo, "Token::kKwDo"},
    {Token::kKwElse, "Token::kKwElse"},
    {Token::kKwElseIf, "Token::kKwElseIf"},
    {Token::kKwEnd, "Token::kKwEnd"},
    {Token::kKwFalse, "Token::kKwFalse"},
    {Token::kKwFor, "Token::kKwFor"},
    {Token::kKwFunction, "Token::kKwFunction"},
    {Token::kKwGoto, "Token::kKwGoto"},
    {Token::kKwIf, "Token::kKwIf"},
    {Token::kKwIn, "Token::kKwIn"},
    {Token::kKwLocal, "Token::kKwLocal"},
    {Token::kKwNil, "Token::kKwNil"},
    {Token::kKwRepeat, "Token::kKwRepeat"},
    {Token::kKwReturn, "Token::kKwReturn"},
    {Token::kKwThen, "Token::kKwThen"},
    {Token::kKwTrue, "Token::kKwTrue"},
    {Token::kKwUntil, "Token::kKwUntil"},
    {Token::kKwWhile, "Token::kKwWhile"},
    {Token::kIdentifier, "Token::kIdentifier"},
    {Token::kNumber, "Token::kNumber"},
    {Token::kString, "Token::kString"},
};

const Token kOpUnM = Token::kOpMinus; // unary minus
const Token kOpSub = Token::kOpMinus;
const Token kOpBNot = Token::kOpWave;
const Token kOpBXor = Token::kOpWave;

const std::map<std::string, Token> kKeywords{ // NOLINT(cert-err58-cpp)
    {"and", Token::kOpAnd},
    {"break", Token::kKwBreak},
    {"do", Token::kKwDo},
    {"else", Token::kKwElse},
    {"elseif", Token::kKwElseIf},
    {"end", Token::kKwEnd},
    {"false", Token::kKwFalse},
    {"for", Token::kKwFor},
    {"function", Token::kKwFunction},
    {"goto", Token::kKwGoto},
    {"if", Token::kKwIf},
    {"in", Token::kKwIn},
    {"local", Token::kKwLocal},
    {"nil", Token::kKwNil},
    {"not", Token::kOpNot},
    {"or", Token::kOpOr},
    {"repeat", Token::kKwRepeat},
    {"return", Token::kKwReturn},
    {"then", Token::kKwThen},
    {"true", Token::kKwTrue},
    {"until", Token::kKwUntil},
    {"while", Token::kKwWhile},
};

std::string GetTokenName(Token token);
}
#endif //EMBRYO_INCLUDE_EMBRYO_LEXER_TOKEN_H_
