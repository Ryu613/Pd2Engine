#pragma once

#include "pd/pd2_engine_export.h"

// DO NOT USE THIS, should explicitly declare within class!
#define MOVABLE_ONLY(CLASS_NAME)                     \
  CLASS_NAME(const CLASS_NAME&) = delete;            \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete; \
  CLASS_NAME(CLASS_NAME&&) noexcept = default;       \
  CLASS_NAME& operator=(CLASS_NAME&&) noexcept = default;

// this is ok!
#define NO_COPY_MOVE(CLASS_NAME)                     \
  CLASS_NAME(const CLASS_NAME&) = delete;            \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete; \
  CLASS_NAME(CLASS_NAME&&) noexcept = delete;        \
  CLASS_NAME& operator=(CLASS_NAME&&) noexcept = delete;

#define PD2_ENGINE_API PD2_ENGINE_EXPORT

namespace pd {
namespace global {
// window default properties
inline const cstr DefaultWindowTitle{"PDEngine"};
inline constexpr u32 DefaultWindowWidth = 1024;
inline constexpr u32 DefaultWindowHeight = 768;
#ifdef NDEBUG
inline constexpr bool enableDebug = false;
#else
inline constexpr bool enableDebug = true;
#endif

}  // namespace global

// helper functions
template <typename Enum>
  requires std::is_enum_v<Enum>
inline std::string GetEnumName(Enum e) {
  auto name = magic_enum::enum_name(e);
  if (!name.empty()) {
    return std::string(name);
  }
  // 如果枚举值超出了反射范围，则降级打印其底层的数字
  return std::to_string(std::to_underlying(e));
}
}  // namespace pd