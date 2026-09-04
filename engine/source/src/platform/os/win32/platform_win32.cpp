#include "pd/platform/platform.hpp"

namespace pd {
void Platform::processEvents() noexcept {
    mWindowSystem.processEvents();
}
}  // namespace pd