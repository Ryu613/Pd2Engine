#include "pd/platform/platform.hpp"

#include "pd/platform/fs/std_file_system.hpp"

namespace pd {
namespace {
std::unique_ptr<IFileSystem> createFileSystem() { return std::make_unique<StdFileSystem>(); }
}  // namespace

Platform::Platform(const PlatformConfig& config)
    : mConfig(config),
      mFileSystem(createFileSystem()) {}

Platform::~Platform() {}

Result<void> Platform::init() noexcept {
  if (auto res = mWindowSystem.init(mConfig.window); !res) {
    return res;
  }

  return {};
}

Result<void> Platform::destroy() noexcept {
  if (auto res = mWindowSystem.destroy(); !res) {
    return res;
  }

  return {};
}
}  // namespace pd