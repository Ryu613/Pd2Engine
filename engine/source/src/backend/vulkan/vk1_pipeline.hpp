#pragma once

#include "vk1_common.hpp"

namespace vk1 {
struct Vk1Pipeline {
  VkPipelineLayout layout{VK_NULL_HANDLE};
  VkPipeline handle = VK_NULL_HANDLE;
};
}  // namespace vk1