#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"
#include "pd/backend/hw_buffer.hpp"
#include "pd/core/error.hpp"

namespace pd {
class IWindow;
class IBackend {
 public:
  enum class GraphicsApi : u8 {
    Noop = 0,
    Vulkan = 1,
    D3D12 = 2,
    Unknown = u8_max,
  };
  struct Config {
    IWindow* pWindow = nullptr;
    bool enableDebug = true;
  };
  IBackend() noexcept = default;
  virtual ~IBackend() = default;
  DELETE_COPY(IBackend);
  DEFAULT_MOVABLE(IBackend);

  [[nodiscard]] virtual GraphicsApi graphicsApi() const noexcept {
    return GraphicsApi::Unknown;
  }

  // swapchain
  [[nodiscard]] virtual HwHandle<Swapchain_t> createSwapchain(
      const HwSwapchain& swapchain) noexcept {
    return {};
  }
  virtual void destroySwapchain(const HwHandle<Swapchain_t>& handle) noexcept {}

  virtual Result<void> newFrame(HwHandle<Swapchain_t>& handle) noexcept {
    return make_error<void>(ErrorCode::RenderingNotImplemented);
  }

  virtual Result<void> presentFrame(HwHandle<Swapchain_t>& handle) noexcept {
    return make_error<void>(ErrorCode::RenderingNotImplemented);
  }

  virtual Result<void> endFrame(HwHandle<Swapchain_t>& handle) noexcept {
    return make_error<void>(ErrorCode::RenderingNotImplemented);
  }

  //   virtual void draw(HwHandle<>)

  // buffer

  [[nodiscard]] virtual HwHandle<Buffer_t> createBuffer(const HwBuffer& buffer) noexcept {
    return {};
  }

  virtual void destroyBuffer(const HwHandle<Buffer_t>& handle) noexcept {}

  virtual void writeBuffer(const BufferWriteOptions& writeOptions) noexcept {}
};
}  // namespace pd