#include "pd/backend/vulkan/vulkan_swapchain.hpp"

namespace pd {
Result<u32> VulkanSwapchain::acquireNextImage(VkDevice device,
                                              VkSemaphore acquireSemaphore) noexcept {
  auto result =
      vkAcquireNextImageKHR(device, handle, u64_max, acquireSemaphore, 0, &currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    // resize();
  }
  return currentImageIndex;
}

Result<void> VulkanSwapchain::present(VkQueue queue, std::span<VkSemaphore> waitSemaphores,
                                      VkFence fence) noexcept {
  VkPresentInfoKHR presentInfo{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = static_cast<u32>(waitSemaphores.size()),
      .pWaitSemaphores = waitSemaphores.data(),
      .swapchainCount = 1,
      .pSwapchains = &handle,
      .pImageIndices = &currentImageIndex,
  };
  auto result = vkQueuePresentKHR(queue, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    // resize();
  }
  return {};
}
}  // namespace pd