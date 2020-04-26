//
// Created by 方泓睿 on 2020/4/22.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_STD_LIB_STR_H_
#define EMBRYO_INCLUDE_EMBRYO_STD_LIB_STR_H_

#include <string>
#include <vector>
#include <tuple>

namespace embryo::std_lib {
std::vector<std::string> ParseFmtStr(const std::string &fmt);

std::tuple<int64_t, int64_t> Find(const std::string &s, const std::string &pattern, size_t init, bool plain);

std::vector<size_t> Match(const std::string &s, const std::string &pattern, size_t init);

std::tuple<std::string, size_t> GSub(const std::string &s, std::string &pattern, const std::string &repl, int64_t n);
}
#endif //EMBRYO_INCLUDE_EMBRYO_STD_LIB_STR_H_
