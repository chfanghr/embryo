//
// Created by 方泓睿 on 2020/4/16.
//

#include <embryo/compiler/code_gen/fi_to_proto.h>
#include <embryo/compiler/code_gen/cg_exp.h>
#include <embryo/compiler/code_gen/code_gen.h>

namespace embryo::code_gen {
std::shared_ptr<bin_chunk::Prototype> GenProto(const std::shared_ptr<ast::Block> &block) {
  auto fd = std::make_shared<ast::FuncDefExp>(1, block->last_line_, true, block);

  auto fi = std::make_shared<FuncInfo>(nullptr, fd);
  fi->AddLocVar("_ENV", 0);
  CGFuncDefExp(fi, fd, 0);
  auto proto = ToProto(fi->sub_funcs_[0]);
  proto->source = "";
  return proto;
}
}