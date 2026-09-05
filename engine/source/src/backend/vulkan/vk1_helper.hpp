#pragma once

#include "vk1_common.hpp"

namespace vk1 {
inline void checkResult(VkResult result) {
  PD_ASSERT_MSG(result == VK_SUCCESS, string_VkResult(result));
}

namespace helper {
inline VkImageSubresourceRange createImageSubresourceRange(
    const uint32_t baseMipLevel = 0,
    const VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    const std::uint32_t levelCount = 1) {
  return VkImageSubresourceRange{
      .aspectMask = aspectMask,
      .baseMipLevel = baseMipLevel,
      .levelCount = levelCount,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };
}
}  // namespace helper

}  // namespace vk1