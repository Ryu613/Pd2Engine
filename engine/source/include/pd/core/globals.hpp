#pragma once

#include <expected>

#define MOVABLE_ONLY(CLASS_NAME)                     \
  CLASS_NAME(const CLASS_NAME&) = delete;            \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete; \
  CLASS_NAME(CLASS_NAME&&) noexcept = default;       \
  CLASS_NAME& operator=(CLASS_NAME&&) noexcept = default;

#define NO_COPY_MOVE(CLASS_NAME)                     \
  CLASS_NAME(const CLASS_NAME&) = delete;            \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete; \
  CLASS_NAME(CLASS_NAME&&) noexcept = delete;        \
  CLASS_NAME& operator=(CLASS_NAME&&) noexcept = delete;

namespace pd {
namespace global {
// window default properties
inline const std::string DefaultWindowTitle{"PDEngine"};
inline constexpr uint32_t DefaultWindowWidth = 1024;
inline constexpr uint32_t DefaultWindowHeight = 768;

}  // namespace global

// aliases
template <typename Type, typename Err>
using Result = std::expected<Type, Err>;

// helper functions
template <typename Enum>
  requires std::is_enum_v<Enum>
inline std::string GetEnumName(Enum e) {
  auto name = magic_enum::enum_name(e);
  if (!name.empty()) {
    return std::string{name};
  }
  // 如果枚举值超出了反射范围，则降级打印其底层的数字
  return std::to_string(std::to_underlying(e));
}
}  // namespace pd