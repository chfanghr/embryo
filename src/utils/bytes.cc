//
// Created by 方泓睿 on 2020/4/7.
//

#include <embryo/utils/bytes.h>

namespace embryo::utils {
std::vector<Byte> Convert(const std::string &src) {
  return std::vector<Byte>(src.begin(), src.end());
}

std::string Convert(const std::vector<Byte> &src) {
  return std::string(src.begin(), src.end());
}
}