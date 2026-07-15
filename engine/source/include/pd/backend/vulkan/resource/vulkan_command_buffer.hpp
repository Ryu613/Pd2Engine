#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource.hpp"
#include "pd/backend/hw_command_recorder.hpp"

namespace pd {
using CommandBuffer_t = CommandRecorder_t;
struct CommandPool_t;
struct VulkanCommandBuffer : public HwCommandRecorder, VulkanResource {
  VulkanCommandBuffer(const HwCommandRecorder& recorder, VulkanResourceManager* mgr,
                      VkCommandPool pool, VkCommandBuffer cmdBfr) noexcept
      : HwCommandRecorder(recorder),
        VulkanResource(mgr),
        pool(pool),
        handle(cmdBfr) {}

  VkCommandPool pool{VK_NULL_HANDLE};
  VkCommandBuffer handle{VK_NULL_HANDLE};
};
}  // namespace pd