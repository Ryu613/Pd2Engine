#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"

namespace pd {
class VulkanResourceManager;
struct VulkanResource {
  VulkanResourceManager* resourceManager = nullptr;
  VmaAllocation allocation{VK_NULL_HANDLE};
};
}  // namespace pd