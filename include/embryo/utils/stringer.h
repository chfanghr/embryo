//
// Created by 方泓睿 on 2020/4/5.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_STRINGER_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_STRINGER_H_

#include <string>

namespace embryo::utils {
class Stringer {
 public:
  virtual ~Stringer() = default;

  [[nodiscard]] virtual std::string String() const = 0;
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_STRINGER_H_
