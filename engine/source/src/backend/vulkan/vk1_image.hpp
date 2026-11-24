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
  std::vector<ImageState> states;

  void transition(VkCommandBuffer cmdBuffer, const ImageState& newState) {
    transition(cmdBuffer, newState, 0, mipLevels);
  }

  void transition(VkCommandBuffer cmdBuffer, const ImageState& newState, uint32_t baseMipLevel,
                  uint32_t mipLevelCount) {
    assert(!states.empty() && baseMipLevel < states.size() && baseMipLevel + mipLevelCount <= states.size());

    const auto endMipLevel = baseMipLevel + mipLevelCount;

    auto runBaseMipLevel = baseMipLevel;
    auto runState = states[runBaseMipLevel];

    // transition images
    const auto transitionRun = [&](const uint32_t runBaseMipLevel, const uint32_t runMipLevelCount,
                                   ImageState runState) {

    };

    for (uint32_t mipLevel = baseMipLevel + 1; mipLevel < endMipLevel; ++mipLevel) {
      if (states[mipLevel] != runState) {
        runBaseMipLevel = mipLevel;
        runState = states[mipLevel];
      }
    }
  }
};

struct Vk1ImageView {
  const Vk1Image* image = nullptr;
  VkImageView imageView = VK_NULL_HANDLE;
};
}  // namespace vk1