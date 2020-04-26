//
// Created by 方泓睿 on 2020/4/16.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_HELPERS_H_
#define EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_HELPERS_H_

#include <memory>
#include <optional>

#include <embryo/compiler/ast/exp.h>
#include <embryo/compiler/ast/stat.h>

namespace embryo::code_gen {
bool IsVarargOrFuncCall(const std::shared_ptr<ast::Exp> &in);

std::optional<std::vector<std::shared_ptr<ast::Exp>>> RemoveTailNils(const std::vector<std::shared_ptr<ast::Exp>> &exps);

int32_t LineOf(const std::shared_ptr<ast::Exp> &exp);

int32_t LastLineOf(const std::shared_ptr<ast::Exp> &exp);
}
#endif //EMBRYO_INCLUDE_EMBRYO_CODE_GEN_CG_HELPERS_H_
