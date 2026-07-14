#include "pd/backend/vulkan/vulkan_swapchain.hpp"

namespace pd {
Result<u32> VulkanSwapchain::acquireNextImage(VkDevice device,
                                              VkSemaphore acquireSemaphore) noexcept {
  u32 imageIndex = 0;
  auto result =
      vkAcquireNextImageKHR(device, handle, u64_max, acquireSemaphore, 0, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    // resize();
  }
  return imageIndex;
}
}  // namespace pd