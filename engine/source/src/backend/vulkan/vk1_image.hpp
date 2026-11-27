#pragma once

#include "vk1_common.hpp"

namespace vk1 {
struct Vk1Image {
  VkImage handle = VK_NULL_HANDLE;
  VmaAllocation allocation{};
  VmaAllocationInfo allocationInfo{};
  VkImageType type = VK_IMAGE_TYPE_2D;
  VkFormat format;
  VkExtent3D extent;
  uint32_t mipLevels = 1;
  uint32_t arrayLayers = 1;
  // for resource status tracking for future auto barrier
  std::vector<ImageState> states;
};

struct Vk1ImageView {
  const Vk1Image* image = nullptr;
  VkImageView imageView = VK_NULL_HANDLE;
};
}  // namespace vk1