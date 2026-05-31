#pragma once

#include "pd/platform/backend/vulkan/vulkan_common.hpp"

namespace pd {

class VulkanContext {
 public:
  struct QueueInfo {
    uint32_t graphicsQueueFamilyIndex = INVALID_VK_INDEX;
    VkQueue graphicsQueue{VK_NULL_HANDLE};
    uint32_t presentQueueFamilyIndex = INVALID_VK_INDEX;
    VkQueue presentQueue{VK_NULL_HANDLE};
  };

  VulkanContext() noexcept;

  VulkanContext(const VulkanContext&) = delete;
  VulkanContext& operator=(const VulkanContext&) = delete;
  VulkanContext(VulkanContext&&) noexcept = delete;
  VulkanContext& operator=(VulkanContext&&) noexcept = delete;

  ~VulkanContext() noexcept;

  bool initialize(const VulkanConfig& vulkanConfig) noexcept;

  bool initialized() const noexcept { return mInitialized; }

  void shutdown() noexcept;

  VkInstance getVkInstance() const noexcept { return mInstance; }

  VkPhysicalDevice getPhysicalDevice() const noexcept { return mPhysicalDevice; }

  VkDevice getDevice() const noexcept { return mDevice; }

  VkSurfaceKHR getSurface() const noexcept { return mSurface; }

  QueueInfo getQueueInfo() const noexcept { return mQueueInfo; }

  VmaAllocator getVmaAllocator() const noexcept { return mVmaAllocator; }

 private:
  bool mInitialized = false;
  VkInstance mInstance{VK_NULL_HANDLE};
  VkDebugUtilsMessengerEXT mDebugMsgr{VK_NULL_HANDLE};
  VkSurfaceKHR mSurface{VK_NULL_HANDLE};
  // device info
  VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
  VkDevice mDevice{VK_NULL_HANDLE};
  // queue info
  QueueInfo mQueueInfo;
  // vma
  VmaAllocator mVmaAllocator{nullptr};

  bool createSurface(void* nativeWindowHandle) noexcept;
};
}  // namespace pd