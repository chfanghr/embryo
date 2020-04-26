//
// Created by 方泓睿 on 2020/4/16.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_CODE_GEN_FI_TO_PROTO_H_
#define EMBRYO_INCLUDE_EMBRYO_CODE_GEN_FI_TO_PROTO_H_

#include <memory>

#include <embryo/bin_chunk/bin_chunk.h>
#include <embryo/compiler/code_gen/func_info.h>

namespace embryo::code_gen {
std::shared_ptr<bin_chunk::Prototype> ToProto(const std::shared_ptr<FuncInfo> &fi);
}
#endif //EMBRYO_INCLUDE_EMBRYO_CODE_GEN_FI_TO_PROTO_H_
