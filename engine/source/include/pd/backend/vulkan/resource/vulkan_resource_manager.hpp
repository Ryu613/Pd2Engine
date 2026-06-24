#pragma once

#include "pd/core/utils/pool.hpp"

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"
#include "pd/backend/hw_buffer.hpp"

#include "pd/backend/vulkan/vulkan_image.hpp"
#include "pd/backend/vulkan/vulkan_swapchain.hpp"
#include "pd/backend/vulkan/vulkan_buffer.hpp"

namespace pd {
class VulkanContext;
/**
 * @brief 统一管理vulkan的资源(除context部分)
 *
 */
class VulkanResourceManager {
 public:
  template <typename T>
  using Handle = HwHandle<T>;

  VulkanResourceManager() noexcept = default;
  explicit VulkanResourceManager(VulkanContext* ctx) noexcept;
  ~VulkanResourceManager() = default;
  MOVABLE_ONLY(VulkanResourceManager);

  Handle<Swapchain_t> createSwapchain(const HwSwapchain& swapchain) noexcept;
  void destroySwapchain(const Handle<Swapchain_t>& handle) noexcept;
  [[nodiscard]] VulkanSwapchain* getSwapchain(
      const Handle<Swapchain_t>& handle) const noexcept;

  Handle<Texture_t> createImage(const HwTexture& texture) noexcept;
  void destroyImage(const Handle<Texture_t>& handle) noexcept;
  [[nodiscard]] VulkanImage* getImage(const Handle<Texture_t>& handle) const noexcept;

  HwHandle<Buffer_t> createBuffer(const HwBuffer& buffer) noexcept;
  void destroyBuffer(const Handle<Buffer_t>& handle) noexcept;

 private:
  VulkanContext* mVulkanContext = nullptr;
  Pool<VulkanImage, Texture_t> mTextures{128};
  Pool<VulkanSwapchain, Swapchain_t> mSwapchains{1};
  Pool<VulkanBuffer, Buffer_t> mBuffers{128};

  template <typename T>
  void setObjectName(VkDevice device, VkObjectType type, T handle,
                     std::string_view name) noexcept {
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