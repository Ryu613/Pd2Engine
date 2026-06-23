#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource.hpp"
#include "pd/backend/hw_buffer.hpp"

namespace pd {
struct VulkanBuffer : public HwBuffer, VulkanResource {
  VulkanBuffer(const HwBuffer& texture, VulkanResourceManager* mgr, VkBuffer buffer,
               VmaAllocation vmaAllocation) noexcept
      : HwBuffer(texture),
        VulkanResource(mgr, vmaAllocation),
        handle(buffer) {}

  VkBuffer handle{VK_NULL_HANDLE};
};
}  // namespace pd