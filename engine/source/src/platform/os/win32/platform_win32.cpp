#include "pd/platform/os/win32/platform_win32.hpp"

#include "pd/platform/file/std_file_system.hpp"

#ifdef WINDOW_SDL3
#include "pd/platform/window/sdl3/window_sdl3.hpp"
#endif

namespace pd {
PlatformWin32::PlatformWin32(Platform::Config config) noexcept
    : mConfig(std::move(config)) {
// TODO(author): windows raw window system not implemented yet
#ifdef WINDOW_SDL3
  mWindow = std::make_unique<WindowSDL3>(mConfig.window);
#else
  PD_ASSERT_MSG(false, "NOT IMPLEMENTED WINDOW SYSTEM");
#endif

  mFileSystem = std::make_unique<StdFileSystem>();
}
PlatformWin32::~PlatformWin32() noexcept {}
}  // namespace pd