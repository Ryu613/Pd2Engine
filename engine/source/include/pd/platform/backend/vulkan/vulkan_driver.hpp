#pragma once

#include "pd/platform/backend/driver.hpp"

#include "pd/platform/backend/vulkan/vulkan_common.hpp"
#include "pd/platform/backend/vulkan/vulkan_context.hpp"
#include "pd/platform/backend/vulkan/vulkan_swapchain.hpp"
#include "pd/platform/backend/vulkan/vulkan_frame.hpp"

namespace pd {
class VulkanDriver : public Driver {
 public:
  explicit VulkanDriver(VulkanConfig config) noexcept;
  ~VulkanDriver();

  // 推进到下一帧
  void nextFrame() noexcept;

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
  // frame info
  std::array<VulkanFrame, INFLIGHT_FRAME_COUNT> mVulkanFrames;
  size_t mCurrentFrameIndex = 0;
};
}  // namespace pd