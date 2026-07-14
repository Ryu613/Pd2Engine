#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource.hpp"
#include "pd/backend/hw_command_list.hpp"

namespace pd {
struct VulkanCommandBuffer : public HwCommandList, VulkanResource {
  VulkanCommandBuffer(const HwCommandList& hwCmdList, VulkanResourceManager* mgr,
                      VkCommandBuffer cmdBfr) noexcept
      : HwCommandList(hwCmdList),
        VulkanResource(mgr),
        handle(cmdBfr) {}

  VkCommandBuffer handle{VK_NULL_HANDLE};
};
}  // namespace pd