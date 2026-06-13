#pragma once

#include "pd/platform/platform.hpp"

#ifdef _WIN32
#include "pd/platform/os/win32/platform_win32.hpp"
#endif  // _WIN32

namespace pd {
template <typename... Args>
std::unique_ptr<Platform> createPlatform(Args&&... args) {
#ifdef _WIN32
  return std::make_unique<PlatformWin32>(std::forward<Args>(args)...);
#else
  PD_ASSERT_MSG(false, "Platform Error!");
#endif  // _WIN32
}
}  // namespace pd