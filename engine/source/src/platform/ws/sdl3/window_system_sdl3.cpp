#include "pd/platform/ws/window_system.hpp"

namespace pd {
Result<void> WindowSystem::init(const WindowConfig& config) noexcept { return {}; }
Result<void> WindowSystem::destroy() noexcept { return {}; }

Result<void> WindowSystem::createWindow() noexcept { return {}; }
}  // namespace pd