#include "pd/platform/os/win32/platform_win32.hpp"

#include "pd/platform/file/std_file_system.hpp"

#ifdef WINDOW_SDL3
#include "pd/platform/window/sdl3/window_sdl3.hpp"
#endif

namespace pd {
PlatformWin32::PlatformWin32(IPlatform::Config config) noexcept
    : mConfig(std::move(config)) {
  // TODO(ryu613): windows original window system not implemented yet
  if (mConfig.window.type == WindowType::SDL3) {
#ifdef WINDOW_SDL3
    mWindow = std::make_unique<WindowSDL3>();
#else
    PD_ASSERT_MSG(false, "NOT IMPLEMENTED WINDOW SYSTEM");
#endif
  } else {
    PD_ASSERT_MSG(false, "NOT IMPLEMENTED WINDOW SYSTEM");
  }

  auto windowInitResult = mWindow->init(mConfig.window);
  PD_ASSERT_MSG(windowInitResult, "window init failed!");

  mFileSystem = std::make_unique<StdFileSystem>();
}
PlatformWin32::~PlatformWin32() = default;

void PlatformWin32::processEvents() noexcept { mWindow->processEvent(); }
}  // namespace pd