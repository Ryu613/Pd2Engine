#pragma once

#include "vk1_common.hpp"

namespace vk1 {
struct Vk1Buffer {
  VkBuffer buffer{VK_NULL_HANDLE};
  VmaAllocation allocation{VK_NULL_HANDLE};
  VmaAllocationInfo allocationInfo{};
  VkDeviceAddress deviceAddress{};
};
}  // namespace vk1