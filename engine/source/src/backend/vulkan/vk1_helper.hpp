#pragma once

#include "vk1_common.hpp"
#include <span>

namespace vk1 {
inline void checkResult(VkResult result) { PD_ASSERT_MSG(result == VK_SUCCESS, string_VkResult(result)); }

namespace helper {
template <typename RawHandle>
[[nodiscard]] inline uint64_t rawHandleToUint64(RawHandle handle) {
  if constexpr (std::is_pointer_v<RawHandle>) {
    return reinterpret_cast<uint64_t>(handle);
  } else {
    return static_cast<uint64_t>(handle);
  }
}

inline VkImageSubresourceRange createImageSubresourceRange(
    const uint32_t baseMipLevel = 0, const VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    const std::uint32_t levelCount = 1) {
  return VkImageSubresourceRange{
      .aspectMask = aspectMask,
      .baseMipLevel = baseMipLevel,
      .levelCount = levelCount,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };
}

inline bool contains(std::span<const std::string> range, const std::string& value) {
  return std::ranges::any_of(range, [&value](const std::string& rangeValue) { return rangeValue == value; });
}
}  // namespace helper

}  // namespace vk1