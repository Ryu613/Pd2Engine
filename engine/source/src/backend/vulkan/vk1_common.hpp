#pragma once

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#include "volk.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vk_mem_alloc.h"

namespace vk1 {
namespace global {
inline constexpr uint32_t invalidIndex = UINT32_MAX;
}

struct ImageState {
  VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  VkPipelineStageFlags2 pipelineStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 accessMask = VK_ACCESS_2_NONE;
  VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
};
}  // namespace vk1