#pragma once

#include "pd/backend/backend.hpp"

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/vulkan_context.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource_manager.hpp"
#include "pd/backend/vulkan/vulkan_command_manager.hpp"
#include "pd/backend/vulkan/vulkan_frame.hpp"

namespace pd {
class BackendVulkan : public IBackend {
 public:
  explicit BackendVulkan(std::unique_ptr<VulkanContext>&& ctx) noexcept;
  ~BackendVulkan();
  DELETE_COPY_MOVE(BackendVulkan);

  [[nodiscard]] GraphicsApi graphicsApi() const noexcept override {
    return IBackend::GraphicsApi::Vulkan;
  }

  // in-frame ops
  Result<void> newFrame(HwHandle<Swapchain_t>& handle) noexcept override;
  Result<void> presentFrame(HwHandle<Swapchain_t>& handle) noexcept override;
  Result<void> endFrame(HwHandle<Swapchain_t>& handle) noexcept override;

  // resource ops
  HwHandle<Swapchain_t> createSwapchain(const HwSwapchain& swapchain) noexcept override;
  void destroySwapchain(const HwHandle<Swapchain_t>& handle) noexcept override;

  HwHandle<Buffer_t> createBuffer(const HwBuffer& buffer) noexcept override;
  void destroyBuffer(const HwHandle<Buffer_t>& handle) noexcept override;
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