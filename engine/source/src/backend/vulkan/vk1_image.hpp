#pragma once

#include "vk1_common.hpp"

namespace vk1 {
struct Vk1Image {
  VkImage image = VK_NULL_HANDLE;
  VmaAllocation allocation{};
  uint32_t mipLevels = 1;
  ImageState state;

  void transition(VkCommandBuffer cmdBuffer, const ImageState& newState) {
    transition(cmdBuffer, newState, 0, mipLevels);
  }

  void transition(VkCommandBuffer cmdBuffer, const ImageState& newState, std::uint32_t baseMipLevel,
                  std::uint32_t mipLevelCount) {}
};

struct Vk1ImageView {
  const Vk1Image* image = nullptr;
  VkImageView imageView = VK_NULL_HANDLE;
};
}  // namespace vk1