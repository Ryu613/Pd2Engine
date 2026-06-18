#pragma once

#include "pd/core/utils/pool.hpp"

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"

#include "pd/backend/vulkan/vulkan_image.hpp"
#include "pd/backend/vulkan/vulkan_swapchain.hpp"

namespace pd {
class VulkanContext;
/**
 * @brief 统一管理vulkan的资源(除context部分)
 *
 */
class VulkanResourceManager {
 public:
  template <BaseOfHwResource T>
  using Handle = HwHandle<T>;

  VulkanResourceManager() noexcept = default;
  explicit VulkanResourceManager(VulkanContext* ctx) noexcept;
  ~VulkanResourceManager() = default;
  MOVABLE_ONLY(VulkanResourceManager);

  Handle<HwSwapchain> createSwapchain(const SwapchainOptions& options) noexcept;
  void destroySwapchain(const Handle<HwSwapchain>& handle) noexcept;
  [[nodiscard]] VulkanSwapchain* getSwapchain(const Handle<HwSwapchain>& handle) noexcept;

  Handle<HwTexture> createImage(const TextureOptions& options) noexcept;
  void destroyImage(const Handle<HwTexture>& handle) noexcept;
  [[nodiscard]] VulkanImage* getImage(const Handle<HwTexture>& handle) noexcept;

 private:
  VulkanContext* mVulkanContext = nullptr;
  Pool<VulkanImage, HwTexture> mTextures{128};
  Pool<VulkanSwapchain, HwSwapchain> mSwapchains{2};

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