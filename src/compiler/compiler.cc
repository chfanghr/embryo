//
// Created by 方泓睿 on 2020/4/17.
//

#include <embryo/bin_chunk/bin_chunk.h>
#include <embryo/compiler/parser/parser.h>
#include <embryo/compiler/code_gen/code_gen.h>
#include <embryo/utils/io.h>
#include <embryo/compiler/compiler.h>

namespace embryo::compiler {
template<>
std::shared_ptr<bin_chunk::Prototype> Compile<std::string>(const std::string &chunk, const std::string &chunk_name) {
  return code_gen::GenProto(parser::Parse(chunk, chunk_name));
}

template<>
std::shared_ptr<bin_chunk::Prototype> Compile<std::vector<Byte>>(const std::vector<Byte> &chunk,
                                                                 const std::string &chunk_name) {
  return code_gen::GenProto(parser::Parse(chunk, chunk_name));
}

std::shared_ptr<bin_chunk::Prototype> Compile(const std::string &file) {
  return code_gen::GenProto(parser::Parse(utils::ReadFile(file), file));
}
}