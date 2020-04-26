//
// Created by 方泓睿 on 2020/4/6.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_PARSER_OPTIMIZERS_H_
#define EMBRYO_INCLUDE_EMBRYO_PARSER_OPTIMIZERS_H_

#include <memory>
#include <embryo/compiler/ast/common.h>

namespace embryo::parser {
std::shared_ptr<ast::Exp> LogicalOrOptimizer(const std::shared_ptr<ast::Exp> &in);

std::shared_ptr<ast::Exp> LogicalAndOptimizer(const std::shared_ptr<ast::Exp> &in);

std::shared_ptr<ast::Exp> BitWiseBinOpOptimizer(const std::shared_ptr<ast::Exp> &in);

std::shared_ptr<ast::Exp> ArithBinaryOpOptimizer(const std::shared_ptr<ast::Exp> &in);

std::shared_ptr<ast::Exp> PowOptimizer(const std::shared_ptr<ast::Exp> &in);

std::shared_ptr<ast::Exp> UnaryOpOptimizer(const std::shared_ptr<ast::Exp> &in);
}
#endif //EMBRYO_INCLUDE_EMBRYO_PARSER_OPTIMIZERS_H_
