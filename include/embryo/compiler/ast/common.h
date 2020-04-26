//
// Created by 方泓睿 on 2020/4/5.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_AST_COMMON_H_
#define EMBRYO_INCLUDE_EMBRYO_AST_COMMON_H_

#include <embryo/utils/stringer.h>

namespace embryo::ast {
class Block;

class Stat : public utils::Stringer {
 public:
  virtual ~Stat() = default;
};

class Exp : public Stat {
 public:
  ~Exp() override = default;
}; // for convenience
}
#endif //EMBRYO_INCLUDE_EMBRYO_AST_COMMON_H_
