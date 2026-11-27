#pragma once

#include "vk1_common.hpp"
#include <span>

namespace vk1 {
inline void checkResult(VkResult result) {
  if (result != VK_SUCCESS) {
    LOG_ERROR("{}", string_VkResult(result));
    std::terminate();
  }
}

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

/**
 * @brief Get the Image Barrier Stage object
 * @note only support graphics pipeline for now
 * @note reference from raptor engine
 * @todo i don't really know why mapping works(for now)
 * @param state image state info
 * @return VkPipelineStageFlags2
 */
inline VkPipelineStageFlags2 getBarrierStage(VkAccessFlags2 accessFlag) {
  VkPipelineStageFlags2 flags = 0;
  if ((accessFlag & (VK_ACCESS_2_INDEX_READ_BIT | VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
    flags |= VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;

  if ((accessFlag & (VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT)) != 0) {
    flags |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
  }

  if ((accessFlag & VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT) != 0) flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

  if ((accessFlag &
       (VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR)) != 0)
    flags |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

  if ((accessFlag & (VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
    flags |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

  if ((accessFlag & VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR) != 0)
    flags = VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

  if ((accessFlag & (VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) !=
      0)
    flags |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;

  if ((accessFlag & VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT) != 0) flags |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;

  if ((accessFlag & (VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT)) != 0)
    flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;

  if ((accessFlag & (VK_ACCESS_2_HOST_READ_BIT | VK_ACCESS_2_HOST_WRITE_BIT)) != 0)
    flags |= VK_PIPELINE_STAGE_2_HOST_BIT;

  if (flags == 0) {
    flags = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
  }

  return flags;
}
}  // namespace helper

}  // namespace vk1