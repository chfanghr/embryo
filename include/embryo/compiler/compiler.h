//
// Created by 方泓睿 on 2020/4/17.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_COMPILER_COMPILER_H_
#define EMBRYO_INCLUDE_EMBRYO_COMPILER_COMPILER_H_

#include <string>
#include <memory>
#include <vector>

#include <embryo/utils/bytes.h>

namespace embryo {
namespace bin_chunk {
struct Prototype;
}

namespace compiler {
template<typename T>
std::shared_ptr<bin_chunk::Prototype> Compile(const T &chunk, const std::string &chunk_name);

template<>
std::shared_ptr<bin_chunk::Prototype> Compile<std::string>(const std::string &chunk, const std::string &chunk_name);

template<>
std::shared_ptr<bin_chunk::Prototype> Compile<std::vector<Byte>>(const std::vector<Byte> &chunk,
                                                                 const std::string &chunk_name);

std::shared_ptr<bin_chunk::Prototype> Compile(const std::string &file);
}
}
#endif //EMBRYO_INCLUDE_EMBRYO_COMPILER_COMPILER_H_
