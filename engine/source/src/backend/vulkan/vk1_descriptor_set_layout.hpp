#pragma once

#include "vk1_common.hpp"

namespace vk1 {

struct Vk1DescriptorSetLayout {
  VkDescriptorSetLayout handle = VK_NULL_HANDLE;
  std::vector<VkDescriptorSetLayoutBinding> bindings;
};
}  // namespace vk1