#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/hw_enums.hpp"

namespace pd {
inline VkImageType ToVkImageType(TextureType type) noexcept {
  switch (type) {
    using enum TextureType;
    case TextureType1D:
      return VK_IMAGE_TYPE_1D;
    case TextureType2D:
      return VK_IMAGE_TYPE_2D;
    case TextureType3D:
      return VK_IMAGE_TYPE_3D;
    case TextureTypeCube:
      return VK_IMAGE_TYPE_2D;
    default:
      PD_ASSERT_MSG(false, "texture type error!");
  }
}

inline VkFormat ToVkFormat(TextureFormat format) noexcept {
  switch (format) {
    using enum TextureFormat;
    case RGBA8_Unorm:
      return VK_FORMAT_R8G8B8A8_UNORM;
    case Undefined:
      return VK_FORMAT_UNDEFINED;
    default:
      PD_ASSERT_MSG(false, "texture format error!");
  }
}

inline VkSampleCountFlagBits ToVkSampleFlagBits(uint32_t count) noexcept {
  return static_cast<VkSampleCountFlagBits>(count);
}

inline VkImageUsageFlags GetVkImageUsageFlagBits(TextureUsage usage) noexcept {
  switch (usage) {
    using enum TextureUsage;
    case DefaultUsage:
      return VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    default:
      PD_ASSERT_MSG(false, "texture usage error!");
  }
}

inline VkImageLayout ToVkImageLayout(TextureLayout layout) noexcept {
  switch (layout) {
    using enum TextureLayout;
    case Undefined:
      return VK_IMAGE_LAYOUT_UNDEFINED;
    case General:
      return VK_IMAGE_LAYOUT_GENERAL;
    default:
      PD_ASSERT_MSG(false, "texture layout error!");
  }
}
}  // namespace pd