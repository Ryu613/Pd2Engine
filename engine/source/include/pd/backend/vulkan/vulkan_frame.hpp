#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"

namespace pd {
class VulkanContext;
class VulkanFrame {
 public:
  VulkanFrame() noexcept = default;
  ~VulkanFrame();

  VulkanFrame(const VulkanFrame&) = delete;
  VulkanFrame& operator=(const VulkanFrame&) = delete;
  VulkanFrame(VulkanFrame&&) noexcept;
  VulkanFrame& operator=(VulkanFrame&&) noexcept = delete;

  bool initialize(VulkanContext& context) noexcept;

  bool initialized() const noexcept { return mInitialized; }

  void shutdown() noexcept;

  void startFence() noexcept;

  void resetFence() noexcept;

  VkFence getFrameFence() noexcept { return mFrameFence; }

  VkSemaphore getImageAvailableSemaphore() noexcept { return mImageAvailableSemaphore; }

  VkCommandBuffer getCommandBuffer() noexcept { return mMainCommandBuffer; }

 private:
  bool mInitialized = false;

  VulkanContext* mVulkanContext = nullptr;

  VkCommandPool mCommandPool{VK_NULL_HANDLE};
  VkCommandBuffer mMainCommandBuffer{VK_NULL_HANDLE};

  VkSemaphore mImageAvailableSemaphore{VK_NULL_HANDLE};
  VkFence mFrameFence{VK_NULL_HANDLE};

  bool initFrame() noexcept;
};
}  // namespace pd