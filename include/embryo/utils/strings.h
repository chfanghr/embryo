//
// Created by 方泓睿 on 2020/4/2.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_STRINGS_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_STRINGS_H_

#include <vector>
#include <string>

namespace embryo::utils {
bool HasPrefix(const std::string &str, const std::string &prefix);

bool HasSuffix(const std::string &str, const std::string &suffix);

std::string ReplaceAll(const std::string &str, char old_ch, char new_ch);

std::string ReplaceAll(std::string str, const std::string &old_str, const std::string &new_str);

std::string TrimSpace(const std::string &);

size_t CountAll(const std::string &str, const std::string &sub_str);

std::string UnicodeToUTF8(int32_t rune);

std::string ToLower(std::string);

std::string ToUpper(std::string);

std::string Join(const std::vector<std::string> &vec, const char *delim);

std::string Join(const std::vector<std::string> &vec, const std::string &delim);

std::vector<std::string> Split(const std::string &s, const std::string &sep);
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_STRINGS_H_
