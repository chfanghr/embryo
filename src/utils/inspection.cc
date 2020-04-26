//
// Created by 方泓睿 on 2020/4/6.
//

#include <algorithm>

#ifndef _MSC_VER
#include <cxxabi.h>
#endif

#include <embryo/utils/inspection.h>

namespace embryo::utils {
std::vector<std::string> JsonStrings(const std::vector<std::string> &src) {
  std::vector<std::string> res{};
  std::transform(src.begin(), src.end(),
                 std::back_inserter(res), [](const std::string &str) { return AS_STRING(str); });
  return res;
}

#ifndef _MSC_VER
std::string CppDemangle(const char *abi_name) {
  int status{};
  char *ret = abi::__cxa_demangle(abi_name, nullptr, nullptr, &status);

  if (!ret) return abi_name;

  std::shared_ptr<char> managed_val{};
  managed_val.reset((char *) ret, [](char *mem) { if (mem)free((void *) mem); });

  return std::string(managed_val.get());
}
#else
std::string CppDemangle(const char *abi_name) { return abi_name; }
#endif
}
