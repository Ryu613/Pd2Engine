#pragma once

#include "pd/core/logging.hpp"

#ifdef _MSC_VER
#define PD_DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define PD_DEBUG_BREAK() __builtin_trap()
#else
#define PD_DEBUG_BREAK() std::abort()
#endif

#ifndef NDEBUG
#define PD_ASSERT_ENABLED
#endif

#ifdef PD_ASSERT_ENABLED
#define PD_ASSERT(expr)                                                    \
  ((expr) ? static_cast<void>(0)                                           \
          : (pd::assert_fail(#expr, "", __FILE__, __LINE__, __FUNCTION__), \
             PD_DEBUG_BREAK()))
#define PD_ASSERT_MSG(expr, msg)                                            \
  ((expr) ? static_cast<void>(0)                                            \
          : (pd::assert_fail(#expr, msg, __FILE__, __LINE__, __FUNCTION__), \
             PD_DEBUG_BREAK()))
#else
#define PD_ASSERT(expr) static_cast<void>(0)
#define PD_ASSERT_MSG(expr, msg) static_cast<void>(0)
#endif  // PD_ASSERT_ENABLED

namespace pd {
inline void assert_fail(const char* expr, const char* what, const char* file, int line,
                        const char* function) {
  log::error("assertion fail: '{}'\n\tFile: {}, Line: {}, Function: {}\n\t{}", expr, file,
             line, function, what);
}
}  // namespace pd