#pragma once

#include "pd/platform/rhi/vulkan/vulkan_common.hpp"

namespace pd {
class VulkanContext;
struct SwapChainInfo {
  VkFormat imageFormat{VK_FORMAT_B8G8R8A8_SRGB};
  VkColorSpaceKHR colorSpace{VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  // v-synced mode
  VkPresentModeKHR presentMode{VK_PRESENT_MODE_FIFO_KHR};
  VkExtent2D extent{};

  // swapchain images & views
  std::vector<VkImage> images;
  std::vector<VkImageView> imageViews;

  // depth image
  std::vector<VmaAllocation> depthImageAllocation;
  std::vector<VkImage> depthImages;
  std::vector<VkImageView> depthImageViews;

  // sync objects for each swapchain images
  std::vector<VkSemaphore> renderFinishedSemaphore;
};
class VulkanSwapchain {
 public:
  explicit VulkanSwapchain(const VulkanContext& ctx) noexcept;

  ~VulkanSwapchain();

  VulkanSwapchain(const VulkanSwapchain&) = delete;
  VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
  VulkanSwapchain(VulkanSwapchain&&) noexcept = delete;
  VulkanSwapchain& operator=(VulkanSwapchain&&) noexcept = delete;

  bool initialize(uint32_t width, uint32_t height) noexcept;

  bool initialized() const noexcept { return mInitialized; }

  void shutdown() noexcept;

  bool createSwapchain(uint32_t width, uint32_t height) noexcept;

  void destroySwapchain() noexcept;

  size_t imageCount() const noexcept { return mSwapChainInfo.images.size(); }

  bool acquireNextImage(VkSemaphore imageAvailableSemaphore) noexcept;

  VkSemaphore getCurrentRenderFinishedSemaphore() noexcept {
    return mSwapChainInfo.renderFinishedSemaphore[mCurrentImageIndex];
  }

  uint32_t getCurrentImageIndex() const noexcept { return mCurrentImageIndex; }

  void present() noexcept;

 private:
  bool mInitialized = false;

  const VulkanContext& mContext;
  VkSwapchainKHR mSwapChain{VK_NULL_HANDLE};
  SwapChainInfo mSwapChainInfo;
  uint32_t mCurrentImageIndex = 0;
};
}  // namespace pd