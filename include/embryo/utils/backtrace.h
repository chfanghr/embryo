//
// Created by fanghr on 4/14/20.
//

#ifndef EMBRYO_INCLUDE_EMBRYO_UTILS_BACKTRACE_H_
#define EMBRYO_INCLUDE_EMBRYO_UTILS_BACKTRACE_H_

#include <functional>
#include <cstdlib>

#define BACKTRACE_MAIN(JOB) int main() { \
embryo::utils::DebugEnvironment(JOB); \
return EXIT_SUCCESS; \
}

#ifdef DEBUG
const bool kDebug = true;
#else
const bool kDebug=false;
#endif

namespace embryo::utils {
int DebugEnvironment(const std::function<void()> &, bool is_debug = kDebug);
}
#endif //EMBRYO_INCLUDE_EMBRYO_UTILS_BACKTRACE_H_
