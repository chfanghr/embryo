//
// Created by 方泓睿 on 2020/4/10.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_HASHERS_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_HASHERS_H_

#include <vector>
#include <unordered_map>
#include <tuple>

namespace {
template<typename Hash, typename Iterator>
size_t order_independent_hash(Iterator begin, Iterator end, Hash hasher) {
  std::vector<size_t> hashes;
  for (Iterator it = begin; it != end; ++it)
    hashes.push_back(hasher(*it));
  std::sort(hashes.begin(), hashes.end());
  size_t result = 0;
  for (unsigned long &hash : hashes)
    result ^=
        hash + 0x9e3779b9 + (result << 6) + (result >> 2); // NOLINT(hicpp-signed-bitwise)
  return result;
}
}
namespace std {
template<typename T1, typename T2>
struct hash<std::pair<T1, T2> > {
  using argument_type = std::pair<T1, T2>;
  using result_type = std::size_t;

  result_type operator()(argument_type const &s) const {
    result_type const h1(std::hash<T1>()(s.first));
    result_type const h2(std::hash<T2>()(s.second));
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2)); // NOLINT(hicpp-signed-bitwise)
  }
};

template<typename Key, typename T>
struct hash<std::unordered_map<Key, T> > {
  using argument_type = std::unordered_map<Key, T>;
  using result_type = std::size_t;

  result_type operator()(argument_type const &s) const {
    return order_independent_hash(s.begin(), s.end(), std::hash<std::pair<Key, T> >());
  }
};

template<typename T>
struct hash<vector<T>> {
  using argument_type = std::vector<T>;
  using result_type = std::size_t;

  result_type operator()(argument_type const &vec) const {
    std::size_t seed = vec.size();
    for (auto &i : vec) {
      seed ^= hash<T>()(i) + 0x9e3779b9
          + (seed << 6) + (seed >> 2); // NOLINT(hicpp-signed-bitwise)
    }
    return seed;
  }
};
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_HASHERS_H_
