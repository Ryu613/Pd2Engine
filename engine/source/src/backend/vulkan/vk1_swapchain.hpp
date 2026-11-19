#pragma once

#include "vk1_common.hpp"

namespace vk1 {
class Vk1Device;
class Vk1Swapchain {
 public:
  struct Info {
    VkExtent2D extent{};
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;
    VkSurfaceCapabilitiesKHR surfaceCaps{};
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<ImageState> imageStates;
    std::vector<VkSemaphore> presentSemaphores;
  };
  explicit Vk1Swapchain(Vk1Device& device);
  ~Vk1Swapchain();
  Vk1Swapchain(const Vk1Swapchain&) = delete;
  Vk1Swapchain& operator=(const Vk1Swapchain&) = delete;
  Vk1Swapchain(Vk1Swapchain&& rhs) noexcept
      : mDevice(rhs.mDevice),
        mInfo(std::exchange(rhs.mInfo, {})),
        mSwapchain(std::exchange(rhs.mSwapchain, VK_NULL_HANDLE)) {}

  Vk1Swapchain& operator=(Vk1Swapchain&& rhs) noexcept = delete;

  void create(uint32_t width, uint32_t height);
  void destroy();

  const Info& getInfo() const { return mInfo; }
  VkSwapchainKHR getSwapchain() { return mSwapchain; }

  uint32_t acquireNextImage(VkSemaphore semaphore);
  void present(uint32_t imageIndex, VkSemaphore waitSemaphore);

 private:
  Vk1Device& mDevice;
  Info mInfo;
  VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;

  void initSwapchainInfo(uint32_t width, uint32_t height);

  std::vector<VkSurfaceFormatKHR> getSupportedSurfaceFormats();
  std::vector<VkPresentModeKHR> getSupportedPresentModes();
};
}  // namespace vk1