#pragma once

#include "pd/backend/backend.hpp"

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/vulkan_context.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource_manager.hpp"
#include "pd/backend/vulkan/vulkan_command_manager.hpp"
#include "pd/backend/vulkan/vulkan_frame.hpp"

namespace pd {
/**
 * @brief backend(RHI)接口枢纽，由此类转发操作，与上层只通过HwHandle交互
 * 资源管理由resource manager控制，具体资源也放在resource/目录下
 * 本级目录的其他类只是一系列管理器，只引用资源，不管理资源生命周期
 */
class BackendVulkan : public IBackend {
 public:
  explicit BackendVulkan(std::unique_ptr<VulkanContext>&& ctx) noexcept;
  ~BackendVulkan();
  DELETE_COPY_MOVE(BackendVulkan);

  [[nodiscard]] GraphicsApi graphicsApi() const noexcept override {
    return IBackend::GraphicsApi::Vulkan;
  }

  // in-frame ops
  // 推进到下一帧
  Result<void> newFrame(const HwHandle<Swapchain_t>& hswapchain) noexcept override;
  // 提交当前帧命令
  void submitFrame(const HwHandle<Swapchain_t>& hswapchain,
                   const HwHandle<CommandRecorder_t>& hrecorder) noexcept override;
  // 呈现当前帧图像
  Result<void> presentFrame(const HwHandle<Swapchain_t>& hswapchain) noexcept override;
  // 结束当前帧
  Result<void> endFrame(const HwHandle<Swapchain_t>& hswapchain) noexcept override;

  // command recorder
  HwHandle<CommandRecorder_t> getCommandRecorder() noexcept override;
  void beginCmdRecord(const HwHandle<CommandRecorder_t>& hrecorder) noexcept override;
  void setGraphicsState(const HwHandle<CommandRecorder_t>& hrecorder,
                        const GraphicsState& graphicsState) noexcept override;
  void drawIndexed(const HwHandle<CommandRecorder_t>& hrecorder,
                   const DrawIndexedCommand& cmd) noexcept override;
  void endCmdRecord(const HwHandle<CommandRecorder_t>& hrecorder) noexcept override;

  // resource ops
  HwHandle<Swapchain_t> createSwapchain(const HwSwapchain& hswapchain) noexcept override;
  void destroySwapchain(const HwHandle<Swapchain_t>& hswapchain) noexcept override;

  HwHandle<Buffer_t> createBuffer(const HwBuffer& buffer) noexcept override;
  void destroyBuffer(const HwHandle<Buffer_t>& hbuffer) noexcept override;
  void writeBuffer(const BufferWriteOptions& writeOptions) noexcept override;

  //   RhiHandle<RhiTexture> createTexture(uint32_t width, uint32_t height,
  //                                       uint32_t depth) noexcept override;

  //   void updateTexture(RhiHandle<RhiTexture> handle, void* pPixelData) noexcept
  //   override;

  //   void destroyTexture(RhiHandle<RhiTexture> handle) noexcept override;

  //   // 推进到下一帧
  //   void acquireNextFrame() noexcept;

  //   // 开始命令录制
  //   void startCmdRecording() noexcept;

  //   // 结束命令录制
  //   void endCmdRecording() noexcept;

  //   // 提交命令
  //   void submitFrame() noexcept;

  //   // 呈现当前帧图像
  //   void presentFrame() noexcept;

  //   // 结束当前帧
  //   void endFrame() noexcept;

  //   VulkanFrame& getCurrentFrame() noexcept { return
  //   mVulkanFrames[mCurrentFrameIndex];
  //   }

 private:
  std::unique_ptr<VulkanContext> mVulkanContext;
  VulkanResourceManager mVulkanResourceManager;
  VulkanCommandManager mVulkanCmdMgr;
  u32 currentFrameIndex = 0;
  std::array<VulkanFrame, global::InFlightFrameCount> mFrames;
  //   size_t mCurrentFrameIndex = 0;
};
}  // namespace pd