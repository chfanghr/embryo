//
// Created by fanghr on 4/14/20.
//

//#include <execinfo.h>
#include <embryo/utils/backtrace.h>
#include <array>
#include <stdexcept>

#include <fmt/format.h>

#include <embryo/utils/inspection.h>

//#define BT_BUF_SIZE 10000

namespace embryo::utils {
int DebugEnvironment(const std::function<void()> &job, bool is_debug) {
  if (!is_debug) {
    job();
    return EXIT_SUCCESS;
  }

  try {
    job();
  } catch (const std::exception &exception) {
    fmt::print(MSG_WITH_FUNC_NAME(
                   fmt::format("exception caught: {}\n", exception.what())));

//    std::array<void *, BT_BUF_SIZE> buffer{};
//
//    auto n_ptr = backtrace(buffer.data(), BT_BUF_SIZE);
//    fmt::print(MSG_WITH_FUNC_NAME(
//                   fmt::format("backtrace() returned {} addresses\n", n_ptr)));
//
//    auto strings = backtrace_symbols(buffer.data(), n_ptr);
//    if (!strings)
//      std::throw_with_nested(
//          MSG_WITH_FUNC_NAME("backtrace_symbols returns nothing"));
//
//    for (size_t i = 0; i < n_ptr; i++)
//      fmt::print("[{:>03}] {}\n", i, strings[i]);
//
//    free(strings);
//
//    fmt::print(MSG_WITH_FUNC_NAME("backtrace done"));
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
}