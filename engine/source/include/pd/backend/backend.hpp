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

  virtual HwHandle<Swapchain_t> createSwapchain(const HwSwapchain& swapchain) noexcept {
    return HwHandle<Swapchain_t>{};
  }
  virtual void destroySwapchain(const HwHandle<Swapchain_t>& handle) noexcept {}
};
}  // namespace pd