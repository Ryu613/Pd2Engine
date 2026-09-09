#include "frame_manager.hpp"

namespace vk1 {

void FrameManager::Frame::init(Vk1Device& device, size_t index) {
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

void FrameManager::Frame::destroy() {
  pDevice->destroyImageView(depthImageView);
  pDevice->destroyImage(depthImage);
  pDevice->destroyCommandPool(cmdPool);
  pDevice->destroySemaphore(acquireImageSemaphore);
  pDevice->destroyFence(frameFence);
}

void FrameManager::init() noexcept {
  for (size_t i = 0; i < mFrames.size(); ++i) {
    mFrames[i].init(*mDevice, i);
  }
}

void FrameManager::destroy() noexcept {
  for (auto& frame : mFrames) {
    frame.destroy();
  }
}

FrameData FrameManager::beginFrame() noexcept {
  auto& currentFrame = mFrames[mCurrentFrameIndex];
  currentFrame.isUsing = true;
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
      .frameIndex = static_cast<uint32_t>(mCurrentFrameIndex),
      .swapchainImageIndex = imageIndex,
  };
}

void FrameManager::endFrame(const pd::CommandRecorder recorder) noexcept {
  auto [frameIndex, imageIndex] = recorder.getInfo();
  auto& frame = mFrames[frameIndex];

  replayCommands(recorder);

  vkEndCommandBuffer(frame.mainCmdBuffer);

  // submit commands
  {
    auto vkQueue = mDevice->getQueue();
    auto& swapchainInfo = mDevice->getSwapchainInfo();
    VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const std::array commandBufferInfos = {VkCommandBufferSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = frame.mainCmdBuffer,
    }};
    const std::array waitInfos = {VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = frame.acquireImageSemaphore,
        .stageMask = waitFlags,
    }};
    const std::array signalInfos = {VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = swapchainInfo.presentSemaphores[imageIndex],
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
    mDevice->resetFences(frame.frameFence);
    vkQueueSubmit2(vkQueue, 1, &submitInfo, frame.frameFence);
  }

  mDevice->present(imageIndex, frame.acquireImageSemaphore);

  advanceFrameIndex(frameIndex);
}

void FrameManager::replayCommands(const pd::CommandRecorder& recorder) noexcept {
  auto commands = recorder.getCommands();
  auto [frameIndex, imageIndex] = recorder.getInfo();
  for (auto& cmd : commands) {
    replayCmd(cmd, frameIndex, imageIndex);
  }
}

void FrameManager::replayCmd(const pd::CommandPayload& payload, uint32_t frameIndex,
                             uint32_t imageIndex) noexcept {
  auto& swapchainInfo = mDevice->getSwapchainInfo();
  switch (payload.type) {
    using enum pd::CmdType;
    case BeginRendering: {
      const auto* args = reinterpret_cast<const pd::BeginRenderingArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];
      VkClearValue clearValue{};
      clearValue.color = {36.0f / 255.0f, 10.0f / 255.0f, 48.0f / 255.0f};
      VkClearValue depthClearValue{};
      depthClearValue.depthStencil.depth = 1.0F;
      std::array colorAtts = {VkRenderingAttachmentInfo{
          .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
          .imageView = swapchainInfo.imageViews[imageIndex],
          .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
          .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
          .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
          .clearValue = clearValue,
      }};
      VkRenderingAttachmentInfo depthAtt{
          .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
          .imageView = frame.depthImageView.imageView,
          .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
          .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
          .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
          .clearValue = depthClearValue,
      };
      VkRenderingInfo renderingInfo{
          .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
          .renderArea =
              {
                  .offset = {},
                  .extent = {swapchainInfo.extent.width, swapchainInfo.extent.height},
              },
          .layerCount = 1,
          .colorAttachmentCount = 1,
          .pColorAttachments = colorAtts.data(),
          .pDepthAttachment = &depthAtt,
      };
      vkCmdBeginRendering(frame.mainCmdBuffer, &renderingInfo);
      break;
    }
    case SetViewport: {
      const auto* args = reinterpret_cast<const pd::SetViewportArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];
      VkViewport viewport{
          .x = 0.f,
          .y = static_cast<float>(swapchainInfo.extent.height),
          .width = static_cast<float>(swapchainInfo.extent.width),
          .height = -static_cast<float>(swapchainInfo.extent.height),
          .minDepth = args->minDepth,
          .maxDepth = args->maxDepth,
      };
      vkCmdSetViewport(frame.mainCmdBuffer, 0, 1, &viewport);
      break;
    }
    case SetScissor: {
      const auto* args = reinterpret_cast<const pd::SetScissorArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];
      VkRect2D scissor{};
      scissor.extent.height = static_cast<float>(swapchainInfo.extent.height);
      scissor.extent.width = static_cast<float>(swapchainInfo.extent.width);
      vkCmdSetScissor(frame.mainCmdBuffer, 0, 1, &scissor);
      break;
    }
    case BindPipeline: {
      const auto* args = reinterpret_cast<const pd::SetScissorArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];
      break;
    }
    case EndRendering: {
      const auto* args = reinterpret_cast<const pd::EndRenderingArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];
      vkCmdEndRendering(frame.mainCmdBuffer);
      break;
    }
    case ClearColorImage: {
      const auto* args = reinterpret_cast<const pd::EndRenderingArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];
      VkClearColorValue color{
          .float32 = {1.f, 0.f, 1.f, 1.f},
      };
      VkImageSubresourceRange subResourceRange{
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .levelCount = 1,
          .layerCount = 1,
      };
      vkCmdClearColorImage(frame.mainCmdBuffer, swapchainInfo.images[frameIndex],
                           VK_IMAGE_LAYOUT_GENERAL, &color, 1, &subResourceRange);
      break;
    }
    default:
      PD_ASSERT_MSG(false, "Backend CmdType not supported!");
  }
}

void FrameManager::advanceFrameIndex(uint32_t currentFrameIndex) noexcept {
  auto nextFrameIndex = (mCurrentFrameIndex + 1) % global::maxInflightFrames;
  auto& nextFrame = mFrames[nextFrameIndex];
  while (nextFrame.isUsing) {
  }
  mFrames[currentFrameIndex].isUsing = false;
  mCurrentFrameIndex = nextFrameIndex;
}
}  // namespace vk1