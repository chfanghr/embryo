//
// Created by 方泓睿 on 2020/4/7.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_BYTES_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_BYTES_H_

#include <vector>
#include <array>
#include <cmath>
#include <string>

#include <embryo/utils/types.h>

namespace embryo::utils {
template<typename T1, size_t S, typename T2>
std::array<T1, S> Convert(const std::vector<T2> &vec) {
  std::array<T1, S> res{};
  for (size_t i = 0; i < std::min(S, vec.size()); i++)
    res[i] = vec[i];
  return res;
}

template<typename T1, size_t S, typename T2>
std::vector<T1> Convert(const std::array<T2, S> &arr) {
  return std::vector<T1>(arr.begin(), arr.end());
}

std::vector<Byte> Convert(const std::string &src);

std::string Convert(const std::vector<Byte> &src);
}

#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_BYTES_H_
