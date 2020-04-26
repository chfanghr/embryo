//
// Created by 方泓睿 on 2020/4/16.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CODE_GEN_H_
#define EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CODE_GEN_H_

#include <memory>

#include <embryo/bin_chunk/bin_chunk.h>
#include <embryo/compiler/ast/block.h>

namespace embryo::code_gen {
std::shared_ptr<bin_chunk::Prototype> GenProto(const std::shared_ptr<ast::Block> &block);
}

#endif //EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CODE_GEN_H_
