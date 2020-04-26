//
// Created by 方泓睿 on 2020/4/5.
//

#include <embryo/compiler/lexer/token.h>

namespace embryo::lexer {
std::string GetTokenName(Token token) {
  auto res = kTokenNames.find(token);
  if (res != kTokenNames.end())
    return res->second;
  throw std::runtime_error("no such token");
}
}