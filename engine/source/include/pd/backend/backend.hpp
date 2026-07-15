#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"
#include "pd/backend/hw_buffer.hpp"
#include "pd/backend/hw_command_recorder.hpp"
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
  virtual void destroySwapchain(const HwHandle<Swapchain_t>& hswapchain) noexcept {}

  virtual Result<void> newFrame(const HwHandle<Swapchain_t>& hswapchain) noexcept {
    return make_error<void>(ErrorCode::RenderingNotImplemented);
  }

  virtual void submitFrame(const HwHandle<Swapchain_t>& hswapchain,
                           const HwHandle<CommandRecorder_t>& hrecorder) noexcept {}

  virtual Result<void> presentFrame(const HwHandle<Swapchain_t>& hswapchain) noexcept {
    return make_error<void>(ErrorCode::RenderingNotImplemented);
  }

  virtual Result<void> endFrame(const HwHandle<Swapchain_t>& hswapchain) noexcept {
    return make_error<void>(ErrorCode::RenderingNotImplemented);
  }

  virtual HwHandle<CommandRecorder_t> getCommandRecorder() noexcept { return {}; }

  virtual void beginCmdRecord(const HwHandle<CommandRecorder_t>& hrecorder) noexcept {}
  virtual void setGraphicsState(const HwHandle<CommandRecorder_t>& hrecorder,
                                const GraphicsState& graphicsState) noexcept {}
  virtual void drawIndexed(const HwHandle<CommandRecorder_t>& hrecorder,
                           const DrawIndexedCommand& cmd) noexcept {}
  virtual void endCmdRecord(const HwHandle<CommandRecorder_t>& hrecorder) noexcept {}

  // buffer

  [[nodiscard]] virtual HwHandle<Buffer_t> createBuffer(const HwBuffer& buffer) noexcept {
    return {};
  }

  virtual void destroyBuffer(const HwHandle<Buffer_t>& hbuffer) noexcept {}

  virtual void writeBuffer(const BufferWriteOptions& writeOptions) noexcept {}
};
}  // namespace pd