#pragma once

#include "pd/platform/rhi/rhi_api.hpp"

#include "pd/platform/rhi/vulkan/vulkan_common.hpp"
#include "pd/platform/rhi/vulkan/vulkan_context.hpp"
#include "pd/platform/rhi/vulkan/vulkan_swapchain.hpp"
#include "pd/platform/rhi/vulkan/vulkan_frame.hpp"
#include "pd/platform/rhi/vulkan/resource/vulkan_resource_manager.hpp"

namespace pd {
class RhiVulkan : public RhiApi {
 public:
  explicit RhiVulkan(VulkanConfig config) noexcept;
  ~RhiVulkan();
  NO_COPY_MOVE(RhiVulkan);

  RhiHandle<RhiTexture> createTexture(uint32_t width, uint32_t height,
                                      uint32_t depth) noexcept override;

  void updateTexture(RhiHandle<RhiTexture> handle, void* pPixelData) noexcept override;

  void destroyTexture(RhiHandle<RhiTexture> handle) noexcept override;

  // 推进到下一帧
  void acquireNextFrame() noexcept;

  // 开始命令录制
  void startCmdRecording() noexcept;

  // 结束命令录制
  void endCmdRecording() noexcept;

  // 提交命令
  void submitFrame() noexcept;

  // 呈现当前帧图像
  void presentFrame() noexcept;

  // 结束当前帧
  void endFrame() noexcept;

  VulkanFrame& getCurrentFrame() noexcept { return mVulkanFrames[mCurrentFrameIndex]; }

 private:
  VulkanConfig mConfig;
  VulkanContext mVulkanContext;
  VulkanSwapchain mVulkanSwapchain;
  VulkanResourceManager mResourceManager;
  // frame info
  std::array<VulkanFrame, INFLIGHT_FRAME_COUNT> mVulkanFrames;
  size_t mCurrentFrameIndex = 0;
};
}  // namespace pd