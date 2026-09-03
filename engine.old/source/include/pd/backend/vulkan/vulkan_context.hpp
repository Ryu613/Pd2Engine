#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"

namespace pd {

class VulkanContext {
 public:
  struct Config {
    std::string appName = global::DefaultWindowTitle;
    uint32_t windowWidth = global::DefaultWindowWidth;
    uint32_t windowHeight = global::DefaultWindowHeight;
    std::vector<const char*> requiredInstanceExtensions;
    void* nativeWindowHandle = nullptr;
    bool enableValidationLayer = true;
  };
  struct QueueInfo {
    uint32_t graphicsQueueFamilyIndex = global::VkInvalidIndex;
    VkQueue graphicsQueue{VK_NULL_HANDLE};
    uint32_t presentQueueFamilyIndex = global::VkInvalidIndex;
    VkQueue presentQueue{VK_NULL_HANDLE};
  };
  explicit VulkanContext(Config config) noexcept;
  ~VulkanContext();
  DELETE_COPY(VulkanContext);
  DEFAULT_MOVABLE(VulkanContext);

  [[nodiscard]] VkInstance getVkInstance() const noexcept { return mInstance; }

  [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const noexcept { return mPhysicalDevice; }

  [[nodiscard]] VkDevice getDevice() const noexcept { return mDevice; }

  [[nodiscard]] VkSurfaceKHR getSurface() const noexcept { return mSurface; }

  [[nodiscard]] QueueInfo getQueueInfo() const noexcept { return mQueueInfo; }

  [[nodiscard]] VmaAllocator getVmaAllocator() const noexcept { return mVmaAllocator; }

 private:
  Config mConfig;
  VkInstance mInstance{VK_NULL_HANDLE};
  VkDebugUtilsMessengerEXT mDebugMsgr{VK_NULL_HANDLE};
  VkSurfaceKHR mSurface{VK_NULL_HANDLE};
  // device info
  VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
  VkDevice mDevice{VK_NULL_HANDLE};
  // vma
  VmaAllocator mVmaAllocator{nullptr};
  // queue info
  QueueInfo mQueueInfo;

  bool createSurface(void* nativeWindowHandle) noexcept;
  void init() noexcept;
  void destroy() noexcept;
  std::vector<const char*> getInstanceExtensions() const noexcept;
};
}  // namespace pd