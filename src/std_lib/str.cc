//
// Created by 方泓睿 on 2020/4/22.
//

#include <regex>
#include <embryo/std_lib/str.h>

namespace embryo::std_lib {
namespace {
std::regex TagPattern(R"(%[ #+-0]?[0-9]*(\.[0-9]+)?[cdeEfgGioqsuxX%])"); // NOLINT(cert-err58-cpp)

std::regex Compile(const std::string &pattern) {
  return std::regex(pattern); // TODO
}

std::vector<size_t> FindStringSubmatchIndex(const std::regex &regex, const std::string &str) {
  std::smatch m{};
  std::regex_match(str, m, regex);
  std::vector<size_t> res(m.size());
  for (size_t i = 0; i < m.size(); ++i)
    res[i] = m.position(i);
  return res;
}

std::vector<std::tuple<size_t, size_t>> FindAllStringIndex(const std::regex &regex, const std::string &str, int64_t n) {
  std::smatch m{};
  std::regex_match(str, m, regex);
  std::vector<std::tuple<size_t, size_t>> res(n < 0 ? m.size() : std::min<size_t>(n, m.size()));
  for (size_t i = 0; i < res.size(); ++i)
    res[i] = {m.position(i), m.position(i) + m[i].length()};

  return res;
}
}

std::vector<std::string> ParseFmtStr(const std::string &fmt) {
  if (fmt.empty() || fmt.find('%') == std::string::npos)
    return {fmt};

  std::vector<std::string> parsed{};
  parsed.reserve(fmt.size() / 2);

  auto iter_begin = std::sregex_iterator(fmt.begin(), fmt.end(), TagPattern);
  auto iter_end = std::sregex_iterator();

  std::for_each(iter_begin,
                iter_end,
                [&](const std::sregex_iterator::value_type &match) { parsed.push_back(match.str()); });

  return parsed;
}

std::tuple<int64_t, int64_t> Find(const std::string &s, const std::string &pattern, size_t init, bool plain) {
  auto tail = init > 1 ? s.substr(init - 1) : s;

  int64_t start{}, end{};
  if (plain) {
    start = tail.find(pattern);
    end = start + pattern.size() - 1;
  } else {
    auto re = Compile(pattern);
    std::smatch m{};
    std::regex_search(tail, m, re);
    if (m.empty())
      start = end = -1;
    else {
      start = m.position(0);
      end = m.position(0) + m[0].length();
    }
  }

  if (start >= 0) {
    start += s.size() - tail.size() + 1;
    end += s.size() - tail.size() + 1;
  }

  return {start, end};
}

std::vector<size_t> Match(const std::string &s, const std::string &pattern, size_t init) {
  auto tail = init > 1 ? s.substr(init - 1) : s;
  auto re = Compile(pattern);
  auto found = FindStringSubmatchIndex(re, tail);
  if (found.size() > 2)
    return std::vector<size_t>(found.begin() + 2, found.end());
  return found;
}

std::tuple<std::string, size_t> GSub(const std::string &s, std::string &pattern, const std::string &repl, int64_t n) {
  auto re = Compile(pattern);
  std::smatch m{};
  std::regex_match(s, m, re);
  auto indexes = FindAllStringIndex(re, s, n);
  if (indexes.empty())
    return {s, 0};
  auto n_matches = indexes.size();
  auto last_end = std::get<1>(indexes[n_matches - 1]);
  auto head = std::string(s.begin(), s.begin() + last_end);
  auto tail = s.substr(last_end);

  auto new_head = std::regex_replace(head, re, repl);
  return {new_head + tail, n_matches};
}
}