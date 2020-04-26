//
// Created by 方泓睿 on 2020/4/5.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_FORMATTERS_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_FORMATTERS_H_

#include <vector>
#include <type_traits>
#include <memory>
#include <array>
#include <unordered_map>

#include <fmt/format.h>

#include <embryo/utils/stringer.h>
#include <embryo/utils/inspection.h>
#include <embryo/state/lua_value.h>

#define USING_EMBRYO_FORMATTERS() ;

template<typename T>
struct fmt::formatter<T, std::enable_if_t<
    std::is_base_of<embryo::utils::Stringer, T>::value, char>> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template<typename FormatContext>
  auto format(const T &obj, FormatContext &ctx) {
    return format_to(ctx.out(), "{}",
                     obj.String());
  }
};

template<typename T>
struct fmt::formatter<std::shared_ptr<T>,
                      std::enable_if_t<
                          std::is_base_of<embryo::utils::Stringer, T>::value, char>> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template<typename FormatContext>
  auto format(const std::shared_ptr<T> &ptr, FormatContext &ctx) {
    return format_to(ctx.out(), /*"std::shared_ptr<{}> -> {}"*/"{}",
        /*typeid(T).name(),*/ ptr ? ptr->String() : "null");
  }
};

template<typename T>
struct fmt::formatter<std::vector<T>> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template<typename FormatContext>
  auto format(const std::vector<T> &vec, FormatContext &ctx) {
    bool is_first = true;
    std::string res = /*fmt::format("std::vector<{}> ", typeid(T).name()) +*/ "[";
    for (const auto &ele:vec) {
      if (is_first)
        is_first = false;
      else
        res += ",";
      res += fmt::format(" {}", ele);
    }
    res += "]";
    return format_to(ctx.out(), "{}", res);
  }
};

template<typename T, size_t S>
struct fmt::formatter<std::array<T, S>> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template<typename FormatContext>
  auto format(const std::array<T, S> &arr, FormatContext &ctx) {
    bool is_first = true;
    std::string res = /*fmt::format("std::vector<{}> ", typeid(T).name()) +*/ "[";
    for (const auto &ele:arr) {
      if (is_first)
        is_first = false;
      else
        res += ",";
      res += fmt::format(" {}", ele);
    }
    res += "]";
    return format_to(ctx.out(), "{}", res);
  }
};

#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_FORMATTERS_H_
