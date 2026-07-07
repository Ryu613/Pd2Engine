#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"
#include "pd/backend/hw_buffer.hpp"
#include "pd/core/error.hpp"

namespace pd {
class IWindow;
class IBackend {
 public:
  enum class GraphicsApi : uint8_t {
    Vulkan,
    D3D12,
    Noop,
    Unknown,
  };
  struct Config {
    IWindow* pWindow = nullptr;
    bool enableDebug = true;
  };
  IBackend() noexcept = default;
  virtual ~IBackend() = default;
  MOVABLE_ONLY(IBackend);

  [[nodiscard]] virtual GraphicsApi graphicsApi() const noexcept {
    return GraphicsApi::Unknown;
  }

  // swapchain
  [[nodiscard]] virtual HwHandle<Swapchain_t> createSwapchain(
      const HwSwapchain& swapchain) noexcept {
    return {};
  }
  virtual void destroySwapchain(const HwHandle<Swapchain_t>& handle) noexcept {}

  virtual Result<void> acquireNextFrame(HwHandle<Swapchain_t>& handle) noexcept {
    return make_error<void>(ErrorCode::RenderingNotImplemented);
  }

  // buffer

  [[nodiscard]] virtual HwHandle<Buffer_t> createBuffer(const HwBuffer& buffer) noexcept {
    return {};
  }

  virtual void destroyBuffer(const HwHandle<Buffer_t>& handle) noexcept {}

  virtual void writeBuffer(const BufferWriteOptions& writeOptions) noexcept {}
};
}  // namespace pd