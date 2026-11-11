#pragma once

#include "vk1_common.hpp"

namespace vk1 {

struct Vk1BindingLayout {
  VkDescriptorSetLayout handle;
  std::vector<VkDescriptorSetLayoutBinding> bindings;
};
}  // namespace vk1