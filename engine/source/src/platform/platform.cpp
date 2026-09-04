#include "pd/platform/platform.hpp"

#include "pd/platform/fs/std_file_system.hpp"

namespace pd {
namespace {
std::unique_ptr<IFileSystem> createFileSystem() { return std::make_unique<StdFileSystem>(); }
}  // namespace

Platform::Platform(const PlatformConfig& config)
    : mConfig(config),
      mFileSystem(createFileSystem()) {}

Platform::~Platform() = default;

Result<void> Platform::init() noexcept {
  auto windowInitRes = mWindowSystem.init(mConfig.window);
  if (!windowInitRes) {
    return windowInitRes;
  }

  return {};
}

Result<void> Platform::destroy() noexcept {
  auto windowDestroyRes = mWindowSystem.destroy();
  if (!windowDestroyRes) {
    return windowDestroyRes;
  }

  return {};
}
}  // namespace pd