#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource.hpp"
#include "pd/backend/hw_swapchain.hpp"

namespace pd {
class VulkanContext;

struct Swapchain_t;
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

struct VulkanSwapchain : public HwSwapchain, VulkanResource {
  VulkanSwapchain(const HwSwapchain& swapchain, VulkanResourceManager* mgr,
                  VkSwapchainKHR vkHandle, SwapChainInfo info) noexcept
      : HwSwapchain(swapchain),
        VulkanResource(mgr),
        handle(vkHandle),
        swapchainInfo(std::move(info)) {}
  //   bool initialize(uint32_t width, uint32_t height) noexcept;

  //   bool initialized() const noexcept { return mInitialized; }

  //   void shutdown() noexcept;

  //   bool createSwapchain(uint32_t width, uint32_t height) noexcept;

  //   void destroySwapchain() noexcept;

  //   size_t imageCount() const noexcept { return mSwapChainInfo.images.size(); }

  //   bool acquireNextImage(VkSemaphore imageAvailableSemaphore) noexcept;

  //   VkSemaphore getCurrentRenderFinishedSemaphore() noexcept {
  //     return mSwapChainInfo.renderFinishedSemaphore[mCurrentImageIndex];
  //   }

  //   uint32_t getCurrentImageIndex() const noexcept { return mCurrentImageIndex; }

  //   void present() noexcept;

  VkSwapchainKHR handle{VK_NULL_HANDLE};
  SwapChainInfo swapchainInfo;
  //   uint32_t mCurrentImageIndex = 0;
};
}  // namespace pd