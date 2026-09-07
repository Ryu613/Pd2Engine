#pragma once

#include "../vk1_device.hpp"
#include "../vk1_helper.hpp"

#include <array>

namespace vk1 {
struct FrameData {
  uint32_t swapchainImageIndex = global::invalidIndex;
};
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

  FrameData beginFrame() noexcept;
  void endFrame(const FrameData& frameData) noexcept;

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
      checkResult(vkAllocateCommandBuffers(pDevice->getDevice(), &allocInfo, &mainCmdBuffer));

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
        pDevice->setDebugName(mainCmdBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER,
                              std::format("frameCmdBuffer[{}]", frameIndex));
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
    VkCommandBuffer mainCmdBuffer = VK_NULL_HANDLE;
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

template <size_t FrameCount>
inline FrameData FrameManager<FrameCount>::beginFrame() noexcept {
  auto& currentFrame = mFrames[mCurrentFrameIndex];
  auto vkDevice = mDevice->getDevice();

  mDevice->waitFences(currentFrame.frameFence);
  uint32_t imageIndex = mDevice->acquireNextImage(currentFrame.acquireImageSemaphore);
  vkResetCommandPool(vkDevice, currentFrame.cmdPool, 0);

  VkCommandBufferBeginInfo cmdBufferBeginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };
  vkBeginCommandBuffer(currentFrame.mainCmdBuffer, &cmdBufferBeginInfo);

  return {
      .swapchainImageIndex = imageIndex,
  };
}

template <size_t FrameCount>
inline void FrameManager<FrameCount>::endFrame(const FrameData& frameData) noexcept {
  auto& currentFrame = mFrames[mCurrentFrameIndex];

  vkEndCommandBuffer(currentFrame.mainCmdBuffer);

  // submit commands
  {
    auto vkQueue = mDevice->getQueue();
    auto& swapchainInfo = mDevice->getSwapchainInfo();
    VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const std::array commandBufferInfos = {VkCommandBufferSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = currentFrame.mainCmdBuffer,
    }};
    const std::array waitInfos = {VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = currentFrame.acquireImageSemaphore,
        .stageMask = waitFlags,
    }};
    const std::array signalInfos = {VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = swapchainInfo.presentSemaphores[frameData.swapchainImageIndex],
        .stageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    }};
    const VkSubmitInfo2 submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = waitInfos.size(),
        .pWaitSemaphoreInfos = waitInfos.data(),
        .commandBufferInfoCount = commandBufferInfos.size(),
        .pCommandBufferInfos = commandBufferInfos.data(),
        .signalSemaphoreInfoCount = signalInfos.size(),
        .pSignalSemaphoreInfos = signalInfos.data(),
    };
    mDevice->resetFences(currentFrame.frameFence);
    vkQueueSubmit2(vkQueue, 1, &submitInfo, currentFrame.frameFence);
  }

  mDevice->present(frameData.swapchainImageIndex, currentFrame.acquireImageSemaphore);

  mCurrentFrameIndex = (mCurrentFrameIndex + 1) % global::maxInflightFrames;
}

}  // namespace vk1