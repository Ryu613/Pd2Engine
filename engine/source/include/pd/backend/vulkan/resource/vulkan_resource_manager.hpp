#pragma once

#include "pd/core/utils/pool.hpp"

#include "pd/backend/vulkan/vulkan_image.hpp"

namespace pd {
class VulkanContext;
/**
 * @brief 统一管理vulkan的资源(除context部分)
 *
 */
class VulkanResourceManager {
 public:
  template <typename T>
  using Handle = TypedHandle<T>;

  VulkanResourceManager() noexcept = default;
  explicit VulkanResourceManager(VulkanContext* ctx) noexcept;
  ~VulkanResourceManager() = default;
  MOVABLE_ONLY(VulkanResourceManager);

  Handle<Image_t> createImage(const TextureOptions& options) noexcept;
  void destroyImage(const Handle<Image_t>& handle) noexcept;
  [[nodiscard]] VulkanImage* getImage(const Handle<Image_t>& handle) noexcept;

 private:
  VulkanContext* mVulkanContext = nullptr;
  Pool<VulkanImage, Image_t> mTextures{128};

  template <typename T>
  void setObjectName(VkDevice device, VkObjectType type, T handle,
                     const std::string& name) noexcept {
#if VK_EXT_debug_utils
    if (device == VK_NULL_HANDLE || vkSetDebugUtilsObjectNameEXT == nullptr ||
        name.empty()) {
      return;
    }
    const VkDebugUtilsObjectNameInfoEXT nameInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = type,
        .objectHandle = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(handle)),
        .pObjectName = name.data(),
    };
    vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
#endif
  }
};
}  // namespace pd