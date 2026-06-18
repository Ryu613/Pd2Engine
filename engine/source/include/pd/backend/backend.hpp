#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"

namespace pd {
class IWindow;
class IBackend {
 public:
  struct Config {
    IWindow* pWindow = nullptr;
    bool enableDebug = true;
  };
  IBackend() noexcept = default;
  virtual ~IBackend() = default;
  MOVABLE_ONLY(IBackend);

  virtual HwHandle<HwSwapchain> createSwapchain(
      const SwapchainOptions& options) noexcept {
    return HwHandle<HwSwapchain>{};
  }
};
}  // namespace pd