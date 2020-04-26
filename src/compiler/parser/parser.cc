//
// Created by 方泓睿 on 2020/4/4.
//

#include <embryo/utils/bytes.h>
#include <embryo/utils/inspection.h>
#include <embryo/compiler/parser/parser.h>

namespace embryo::parser {
template<>
std::shared_ptr<ast::Block> Parse<std::string>(const std::string &chunk, const std::string &chunk_name) {
  return Parser(chunk, chunk_name).Parse();
}

template<>
std::shared_ptr<ast::Block> Parse<std::vector<Byte>>(const std::vector<Byte> &chunk, const std::string &chunk_name) {
  return Parser(utils::Convert(chunk), chunk_name).Parse();
}

Parser::Parser(const std::string &chunk, const std::string &chunk_name, bool optimize)
    : lexer_(chunk, chunk_name), optimize_(optimize) {}

std::shared_ptr<ast::Block> Parser::Parse() {
  auto block = ParseBlock();
  lexer_.NextTokenOfKind(lexer::Token::kEOF);
  return block;
}

void Parser::Error(const std::string &msg) {
  auto err_msg = fmt::format("{}: (chunk: {}, line: {}) {}",
                             THIS_TYPE, lexer_.ChunkName(), lexer_.Line(), msg);
  throw std::runtime_error(err_msg);
}
}