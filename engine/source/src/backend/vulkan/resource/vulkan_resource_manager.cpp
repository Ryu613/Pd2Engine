#include "pd/backend/vulkan/resource/vulkan_resource_manager.hpp"

#include "pd/backend/vulkan/vulkan_types.hpp"
#include "pd/backend/vulkan/vulkan_context.hpp"

namespace pd {
template <typename T>
using Handle = VulkanResourceManager::Handle<T>;

VulkanResourceManager::VulkanResourceManager(VulkanContext* ctx) noexcept
    : mVulkanContext(ctx) {}

Handle<HwTexture> VulkanResourceManager::createImage(
    const TextureOptions& options) noexcept {
  auto* device = mVulkanContext->getDevice();

  VkImageCreateInfo imageCInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .flags = options.isProtected ? VK_IMAGE_CREATE_PROTECTED_BIT : 0u,
      .imageType = ToVkImageType(options.type),
      .format = ToVkFormat(options.format),
      .extent =
          {
              .width = options.extent.width,
              .height = options.extent.height,
              .depth = options.extent.depth,
          },
      .mipLevels = options.mipLevel,
      .arrayLayers = options.arrayLeyers,
      .samples = ToVkSampleFlagBits(options.samples),
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = GetVkImageUsageFlagBits(options.usage),
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      // queue family index
      .initialLayout = ToVkImageLayout(options.initialLayout),
  };

  if (options.type == TextureType::TextureTypeCube) {
    imageCInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
  }

  VmaAllocationCreateInfo allocCInfo{
      .usage = VMA_MEMORY_USAGE_AUTO,
  };

  // TODO(author): external image?

  auto* allocator = mVulkanContext->getVmaAllocator();
  VkImage vkImage{};
  VmaAllocation vmaAllocation{};
  if (auto result = vmaCreateImage(allocator, &imageCInfo, &allocCInfo, &vkImage,
                                   &vmaAllocation, nullptr);
      result != VK_SUCCESS) {
    log::error("vulkan create image error: {}", string_VkResult(result));
    return {};
  }
  VmaAllocationInfo allocInfo;
  vmaGetAllocationInfo(allocator, vmaAllocation, &allocInfo);

  setObjectName(device, VK_OBJECT_TYPE_IMAGE, vkImage, options.debugLabel);

  const auto textureHandle =
      mTextures.emplace(VulkanImage{options, this, vkImage, vmaAllocation});

  return textureHandle;
}

void VulkanResourceManager::destroyImage(const Handle<HwTexture>& handle) noexcept {
  auto* vulkanImage = mTextures.get(handle);
  auto* allocator = mVulkanContext->getVmaAllocator();
  vmaDestroyImage(allocator, vulkanImage->handle, vulkanImage->allocation);

  mTextures.remove(handle);
}

[[nodiscard]] VulkanImage* VulkanResourceManager::getImage(
    const Handle<HwTexture>& handle) noexcept {
  return mTextures.get(handle);
}

Handle<HwSwapchain> VulkanResourceManager::createSwapchain(
    const SwapchainOptions& options) noexcept {
  return {};
}
void VulkanResourceManager::destroySwapchain(const Handle<HwSwapchain>& handle) noexcept {
}
[[nodiscard]] VulkanSwapchain* VulkanResourceManager::getSwapchain(
    const Handle<HwSwapchain>& handle) noexcept {
  return nullptr;
}
}  // namespace pd