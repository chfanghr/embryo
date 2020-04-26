//
// Created by 方泓睿 on 2020/4/5.
//

#include <algorithm>
#include <locale>
#include <codecvt>
#include <sstream>
#include <regex>

#include <embryo/utils/strings.h>

namespace embryo::utils {

bool HasPrefix(const std::string &str, const std::string &prefix) {
  if (str.size() < prefix.size())return false;
  auto res = std::mismatch(prefix.begin(), prefix.end(), str.begin());
  return res.first == prefix.end();
}

bool HasSuffix(const std::string &str, const std::string &suffix) {
  if (suffix.size() > str.size()) return false;
  return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

std::string ReplaceAll(const std::string &str, char old_ch, char new_ch) {
  auto res = str;
  std::replace(std::begin(res), std::end(res), old_ch, new_ch);
  return res;
}

std::string ReplaceAll(std::string s, const std::string &search, const std::string &replace) {
  for (size_t pos = 0;; pos += replace.length()) {
    pos = s.find(search, pos);
    if (pos == std::string::npos) break;
    s.erase(pos, search.length());
    s.insert(pos, replace);
  }
  return s;
}

size_t CountAll(const std::string &str, const std::string &sub_str) {
  size_t counter{};

  for (size_t pos = str.find(sub_str); pos != std::string::npos; counter++)
    pos = str.find(sub_str, pos + 1);

  return counter;
}

std::string UnicodeToUTF8(int32_t rune) {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
  std::string u8_str = converter.to_bytes(rune);
  return u8_str;
}

const static std::string kWhitespace = " \n\r\t\f\v"; // NOLINT(cert-err58-cpp)

static std::string LTrim(const std::string &s) {
  size_t start = s.find_first_not_of(kWhitespace);
  return (start == std::string::npos) ? "" : s.substr(start);
}

static std::string RTrim(const std::string &s) {
  size_t end = s.find_last_not_of(kWhitespace);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string TrimSpace(const std::string &str) {
  return RTrim(LTrim(str));
}

std::string ToLower(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return str;
}

std::string ToUpper(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return str;
}

std::string Join(const std::vector<std::string> &vec, const char *delim) {
  std::stringstream res{};
  std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(res, delim));
  return res.str();
}

std::string Join(const std::vector<std::string> &vec, const std::string &delim) {
  return Join(vec, delim.c_str());
}

std::vector<std::string> Split(const std::string &s, const std::string &sep) {
  std::vector<std::string> output;
  std::string::size_type prev_pos = 0, pos = 0;
  while ((pos = s.find(sep, pos)) != std::string::npos) {
    std::string substring(s.substr(prev_pos, pos - prev_pos));
    output.push_back(substring);
    prev_pos = ++pos;
  }
  output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
  return output;
}
}