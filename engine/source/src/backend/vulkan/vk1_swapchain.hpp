#pragma once

#include "vk1_common.hpp"

namespace vk1 {
class Vk1Device;
class Vk1Swapchain {
 public:
  struct Info {
    // VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkExtent2D extent{};
    VkFormat format = VK_FORMAT_UNDEFINED;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<ImageState> imageStates;
    VkSemaphore presentSemaphore = VK_NULL_HANDLE;
    // Track the swapchain image layout between frames so the next render pass knows the
    // correct source state for its first barrier.
  };
  ~Vk1Swapchain();
  Vk1Swapchain(const Vk1Swapchain&) = delete;
  Vk1Swapchain& operator=(const Vk1Swapchain&) = delete;
  Vk1Swapchain(Vk1Swapchain&& rhs) noexcept = default;
  Vk1Swapchain& operator=(Vk1Swapchain&& rhs) noexcept = default;

  void create(VkSurfaceKHR surface, uint32_t queueFamilyIndex, uint32_t width, uint32_t height);
  void destroy();

  const Info& getInfo() const { return mInfo; }
  VkSwapchainKHR getSwapchain() { return mSwapchain; }

  uint32_t acquireNextImage(VkSemaphore semaphore);
  void present(uint32_t imageIndex, VkSemaphore waitSemaphore);

 private:
  friend class Vk1Device;
  explicit Vk1Swapchain(Vk1Device& device);
  Info mInfo;
  Vk1Device* mpDevice = nullptr;
  VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
};
}  // namespace vk1