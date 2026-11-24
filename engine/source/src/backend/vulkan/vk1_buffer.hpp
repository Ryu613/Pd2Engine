#pragma once

#include "vk1_common.hpp"

namespace vk1 {
struct Vk1Buffer {
  VkBuffer handle{VK_NULL_HANDLE};
  VkDeviceSize size = 0;
  VmaAllocation allocation{VK_NULL_HANDLE};
  VmaAllocationInfo allocationInfo{};
  VkDeviceAddress deviceAddress{};
};
}  // namespace vk1