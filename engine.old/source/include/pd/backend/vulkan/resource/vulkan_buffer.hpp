#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource.hpp"
#include "pd/backend/hw_buffer.hpp"

namespace pd {
struct VulkanBuffer : public HwBuffer, VulkanResource {
  VulkanBuffer(const HwBuffer& hwBuffer, VulkanResourceManager* mgr, VkBuffer vkBuffer,
               VmaAllocation vmaAllocation, VmaAllocationInfo vmaAllocationInfo) noexcept
      : HwBuffer(hwBuffer),
        VulkanResource(mgr, vmaAllocation, vmaAllocationInfo),
        handle(vkBuffer) {}

  VkBuffer handle{VK_NULL_HANDLE};
};
}  // namespace pd