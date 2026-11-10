#pragma once

#include "vk1_common.hpp"

namespace vk1 {

struct BindingLayout {
  VkDescriptorSetLayout handle;
  std::vector<VkDescriptorSetLayoutBinding> bindings;
};
}  // namespace vk1