//
// Created by 方泓睿 on 2020/4/4.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_AST_BLOCK_H_
#define EMBRYO_INCLUDE_EMBRYO_AST_BLOCK_H_

#include <vector>
#include <memory>

#include <embryo/compiler/ast/common.h>
#include <embryo/utils/stringer.h>

namespace embryo::ast {
// block ::= {stat}
class Block : public utils::Stringer {
 public:
  std::string chunk_name_;
  size_t last_line_{};
  std::vector<std::shared_ptr<Stat>> stats_{};
  std::vector<std::shared_ptr<Exp>> ret_exps_{};

  [[nodiscard]] std::string String() const override;
};
}

#endif //EMBRYO_INCLUDE_EMBRYO_AST_BLOCK_H_
