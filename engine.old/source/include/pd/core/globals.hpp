#pragma once

#include "pd/pd2_engine_export.h"

#define DELETE_COPY(CLASS_NAME)           \
  CLASS_NAME(const CLASS_NAME&) = delete; \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete;

#define DEFAULT_MOVABLE(CLASS_NAME)            \
  CLASS_NAME(CLASS_NAME&&) noexcept = default; \
  CLASS_NAME& operator=(CLASS_NAME&&) noexcept = default;

#define DELETE_COPY_MOVE(CLASS_NAME)                 \
  CLASS_NAME(const CLASS_NAME&) = delete;            \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete; \
  CLASS_NAME(CLASS_NAME&&) noexcept = delete;        \
  CLASS_NAME& operator=(CLASS_NAME&&) noexcept = delete;

#define PD2_ENGINE_API PD2_ENGINE_EXPORT

namespace pd {
namespace global {
#ifdef NDEBUG
inline constexpr bool enableDebug = false;
#else
inline constexpr bool enableDebug = true;
#endif

}  // namespace global
}  // namespace pd