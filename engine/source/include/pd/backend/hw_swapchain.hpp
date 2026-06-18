#pragma once

#include "pd/backend/hw_resource.hpp"

namespace pd {
struct SwapchainOptions {
  uint32_t width = 0;
  uint32_t height = 0;
};

struct HwSwapchain : public HwResource {
  SwapchainOptions options;
};
}  // namespace pd