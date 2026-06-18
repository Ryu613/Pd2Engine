#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource.hpp"
#include "pd/backend/hw_texture.hpp"

namespace pd {
struct Image_t;
struct VulkanImage : public HwTexture, VulkanResource {
  VulkanImage(const TextureOptions& options, VulkanResourceManager* mgr, VkImage img,
              VmaAllocation vmaAllocation) noexcept
      : HwTexture(options),
        VulkanResource(mgr, vmaAllocation),
        handle(img) {}
  VkImage handle{VK_NULL_HANDLE};
};
}  // namespace pd