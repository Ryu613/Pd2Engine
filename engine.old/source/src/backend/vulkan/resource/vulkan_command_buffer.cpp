#include "pd/backend/vulkan/resource/vulkan_command_buffer.hpp"

namespace pd {
void VulkanCommandBuffer::begin() noexcept {
  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };

  VK_CHECK(vkBeginCommandBuffer(handle, &beginInfo));
}
void VulkanCommandBuffer::end() noexcept { VK_CHECK(vkEndCommandBuffer(handle)); }

void VulkanCommandBuffer::submit(VkQueue queue, std::span<VkSemaphore> waitSemaphores,
                                 std::span<VkSemaphore> signalSemaphores,
                                 VkFence fence) noexcept {
  std::array<VkPipelineStageFlags, 1> waitStages{
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
  };
  VkSubmitInfo submitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
      .pWaitSemaphores = waitSemaphores.data(),
      .pWaitDstStageMask = waitStages.data(),
      .commandBufferCount = 1,
      .pCommandBuffers = &handle,
      .signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
      .pSignalSemaphores = signalSemaphores.data(),
  };
  VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence));
}
}  // namespace pd