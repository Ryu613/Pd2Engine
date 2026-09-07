#pragma once

#include "../vk1_device.hpp"
#include "../vk1_helper.hpp"

#include <array>

namespace vk1 {

template <size_t FrameCount>
class FrameManager {
 public:
  explicit FrameManager(Vk1Device& device)
      : mDevice(&device) {}

  ~FrameManager() = default;
  FrameManager(const FrameManager&) = delete;
  FrameManager& operator=(const FrameManager&) = delete;
  FrameManager(FrameManager&& rhs) noexcept = delete;
  FrameManager& operator=(FrameManager&& rhs) noexcept = delete;

  void init() noexcept;
  void destroy() noexcept;

 private:
  struct Frame {
    void init(Vk1Device& device, size_t index) {
      pDevice = &device;
      frameIndex = index;

      // create common handles
      frameFence = pDevice->createFence();
      acquireImageSemaphore = pDevice->createSemaphore();
      cmdPool = pDevice->createCommandPool();
      VkCommandBufferAllocateInfo allocInfo{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
          .commandPool = cmdPool,
          .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
          .commandBufferCount = 1,
      };
      checkResult(vkAllocateCommandBuffers(pDevice->getDevice(), &allocInfo, &frameMainCmdBuffer));

      // create depth image
      auto [width, height] = pDevice->getSwapchainInfo().extent;
      depthImage = pDevice->createImage(VK_FORMAT_D32_SFLOAT, width, height,
                                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
      auto subResourceRange = helper::createImageSubresourceRange(0, VK_IMAGE_ASPECT_DEPTH_BIT);
      depthImageView = pDevice->createImageView(depthImage, VK_FORMAT_D32_SFLOAT, subResourceRange);
      // transition?

      // set debug names
      {
        pDevice->setDebugName(frameFence, VK_OBJECT_TYPE_FENCE,
                              std::format("frameFence[{}]", frameIndex));
        pDevice->setDebugName(acquireImageSemaphore, VK_OBJECT_TYPE_SEMAPHORE,
                              std::format("acquireImageSemaphore[{}]", frameIndex));
        pDevice->setDebugName(cmdPool, VK_OBJECT_TYPE_COMMAND_POOL,
                              std::format("frameCmdPool[{}]", frameIndex));
        pDevice->setDebugName(depthImage.image, VK_OBJECT_TYPE_IMAGE,
                              std::format("depthImage[{}]", frameIndex));
        pDevice->setDebugName(depthImageView.imageView, VK_OBJECT_TYPE_IMAGE_VIEW,
                              std::format("depthImageView[{}]", frameIndex));
      }
    }

    void destroy() {
      pDevice->destroyImageView(depthImageView);
      pDevice->destroyImage(depthImage);
      pDevice->destroyCommandPool(cmdPool);
      pDevice->destroySemaphore(acquireImageSemaphore);
      pDevice->destroyFence(frameFence);
    }

    Vk1Device* pDevice = nullptr;
    size_t frameIndex = 0;
    VkFence frameFence = VK_NULL_HANDLE;
    VkSemaphore acquireImageSemaphore = VK_NULL_HANDLE;
    VkCommandPool cmdPool = VK_NULL_HANDLE;
    VkCommandBuffer frameMainCmdBuffer = VK_NULL_HANDLE;
    Vk1Image depthImage{};
    Vk1ImageView depthImageView{};
  };

  Vk1Device* mDevice = nullptr;
  std::array<Frame, FrameCount> mFrames;
  size_t mCurrentFrameIndex = 0;
};

template <size_t FrameCount>
inline void FrameManager<FrameCount>::init() noexcept {
  for (size_t i = 0; i < mFrames.size(); ++i) {
    mFrames[i].init(*mDevice, i);
  }
}

template <size_t FrameCount>
inline void FrameManager<FrameCount>::destroy() noexcept {
  for (auto& frame : mFrames) {
    frame.destroy();
  }
}

}  // namespace vk1