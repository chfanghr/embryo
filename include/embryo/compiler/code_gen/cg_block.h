//
// Created by 方泓睿 on 2020/4/15.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_BLOCK_H_
#define EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_BLOCK_H_

#include <embryo/compiler/ast/block.h>
#include <embryo/compiler/ast/common.h>
#include <embryo/compiler/code_gen/func_info.h>

namespace embryo::code_gen {
void CGBlock(const std::shared_ptr<FuncInfo> &fi, const std::shared_ptr<ast::Block> &block);

void CGRetStat(const std::shared_ptr<FuncInfo> &fi,
               std::vector<std::shared_ptr<ast::Exp>> &exps,
               int32_t last_line);
}
#endif //EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_BLOCK_H_
