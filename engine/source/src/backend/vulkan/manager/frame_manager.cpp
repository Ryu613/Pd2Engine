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
  depthImage = pDevice->createImage(VK_FORMAT_D32_SFLOAT, width, height, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  auto subResourceRange = helper::createImageSubresourceRange(0, VK_IMAGE_ASPECT_DEPTH_BIT);
  depthImageView = pDevice->createImageView(depthImage, VK_FORMAT_D32_SFLOAT, subResourceRange);

  // create uniform buffer
  uniformBuffer = pDevice->createBuffer(sizeof(pd::UniformBufferObject), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                                        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                            VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                                            VMA_ALLOCATION_CREATE_MAPPED_BIT,
                                        VMA_MEMORY_USAGE_AUTO);

  // set debug names
  {
    pDevice->setDebugName(frameFence, VK_OBJECT_TYPE_FENCE, std::format("frameFence[{}]", frameIndex));
    pDevice->setDebugName(acquireImageSemaphore, VK_OBJECT_TYPE_SEMAPHORE,
                          std::format("acquireImageSemaphore[{}]", frameIndex));
    pDevice->setDebugName(cmdPool, VK_OBJECT_TYPE_COMMAND_POOL, std::format("frameCmdPool[{}]", frameIndex));
    pDevice->setDebugName(mainCmdBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER, std::format("frameCmdBuffer[{}]", frameIndex));
    pDevice->setDebugName(depthImage.image, VK_OBJECT_TYPE_IMAGE, std::format("depthImage[{}]", frameIndex));
    pDevice->setDebugName(depthImageView.imageView, VK_OBJECT_TYPE_IMAGE_VIEW,
                          std::format("depthImageView[{}]", frameIndex));
    pDevice->setDebugName(uniformBuffer.handle, VK_OBJECT_TYPE_BUFFER, std::format("uniformBuffer[{}]", frameIndex));
  }
}

void FrameManager::Frame::update() {
  // shortcut: need camera and delta time
  float aspectRatio = static_cast<float>(1024) / static_cast<float>(768);
  // ubo.model = glm::rotate(ubo.model, 0.003f, glm::vec3{0.0f, 1.0f, 0.0f});
  ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  // ubo.proj = glm::perspective(glm::radians(45.f), aspectRatio, 0.1f, 100.0f);
  // ubo.proj[1][1] *= -1.0F;
  // ubo.proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

  memcpy(uniformBuffer.allocationInfo.pMappedData, &ubo, sizeof(pd::UniformBufferObject));
}

void FrameManager::Frame::destroy() {
  pDevice->destroyBuffer(uniformBuffer);
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

  // update uniform buffer
  currentFrame.update();

  // prepare command buffers
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

void FrameManager::endFrame(const pd::CommandRecorder& recorder) noexcept {
  auto [frameIndex, imageIndex] = recorder.getInfo();
  auto& frame = mFrames[frameIndex];

  replayCommands(recorder);

  checkResult(vkEndCommandBuffer(frame.mainCmdBuffer));

  // submit commands
  auto& swapchainInfo = mDevice->getSwapchainInfo();
  {
    auto vkQueue = mDevice->getQueue();
    VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
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
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
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
    advanceFrameIndex(frameIndex);
    vkQueueSubmit2(vkQueue, 1, &submitInfo, frame.frameFence);
  }

  mDevice->present(imageIndex, swapchainInfo.presentSemaphores[imageIndex]);

  // vkDeviceWaitIdle(mDevice->getDevice());
}

void FrameManager::replayCommands(const pd::CommandRecorder& recorder) noexcept {
  auto commands = recorder.getCommands();
  auto [frameIndex, imageIndex] = recorder.getInfo();
  for (auto& cmd : commands) {
    replayCmd(cmd, frameIndex, imageIndex);
  }
}

void FrameManager::replayCmd(const pd::CommandPayload& payload, uint32_t frameIndex, uint32_t imageIndex) noexcept {
  auto& swapchainInfo = mDevice->getSwapchainInfo();
  switch (payload.type) {
    using enum pd::CmdType;
    case BeginRendering: {
      const auto* args = reinterpret_cast<const pd::BeginRenderingArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];

      // memory barriers
      std::array<VkImageMemoryBarrier2, 2> outputBarriers{
          VkImageMemoryBarrier2{
              .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
              .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
              .srcAccessMask = 0,
              .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
              .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
              .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
              .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
              .image = swapchainInfo.images[imageIndex],
              .subresourceRange{
                  .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                  .levelCount = 1,
                  .layerCount = 1,
              },
          },
          VkImageMemoryBarrier2{
              .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
              .srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
              .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
              .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
              .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
              .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
              .newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
              .image = frame.depthImage.image,
              .subresourceRange{
                  .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                  .levelCount = 1,
                  .layerCount = 1,
              },
          }};
      VkDependencyInfo barrierDependencyInfo{
          .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
          .imageMemoryBarrierCount = outputBarriers.size(),
          .pImageMemoryBarriers = outputBarriers.data(),
      };
      vkCmdPipelineBarrier2(frame.mainCmdBuffer, &barrierDependencyInfo);

      // rendering info
      VkClearValue clearValue{};
      clearValue.color = {36.0f / 255.0f, 10.0f / 255.0f, 48.0f / 255.0f};
      VkClearValue depthClearValue{
          .depthStencil =
              {
                  .depth = 1.0f,
                  .stencil = 0,
              },
      };
      std::array colorAtts = {VkRenderingAttachmentInfo{
          .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
          .imageView = swapchainInfo.imageViews[imageIndex],
          .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
          .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
          .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
          .clearValue = clearValue,
      }};
      VkRenderingAttachmentInfo depthAtt{
          .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
          .imageView = frame.depthImageView.imageView,
          .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
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
          .colorAttachmentCount = colorAtts.size(),
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
      const auto* args = reinterpret_cast<const pd::BindPipelineArgs*>(&payload.args[0]);
      const auto& pipelineHandle = args->pipeline;
      const auto& vertexBufferHandle = args->vertexBuffer;
      const auto& indexBufferHandle = args->indexBuffer;
      const Vk1Pipeline* pip = mRegistry->getResource(pipelineHandle);
      PD_ASSERT_MSG(pip, "pip not exist!");
      const Vk1Buffer* vertexBuffer = mRegistry->getResource(vertexBufferHandle);
      PD_ASSERT_MSG(vertexBuffer, "vertex buffer not exist!");
      const Vk1Buffer* indexBuffer = mRegistry->getResource(indexBufferHandle);
      PD_ASSERT_MSG(indexBuffer, "index buffer not exist!");
      auto& frame = mFrames[frameIndex];
      vkCmdBindPipeline(frame.mainCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pip->handle);
      VkDeviceSize deviceSize{};
      vkCmdBindVertexBuffers(frame.mainCmdBuffer, 0, 1, &vertexBuffer->handle, &deviceSize);
      vkCmdBindIndexBuffer(frame.mainCmdBuffer, indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);
      vkCmdPushConstants(frame.mainCmdBuffer, pip->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress),
                         &frame.uniformBuffer.deviceAddress);
      break;
    }
    case DrawIndexed: {
      const auto* args = reinterpret_cast<const pd::DrawIndexedArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];
      vkCmdDrawIndexed(frame.mainCmdBuffer, args->indexCount, args->instanceCount, args->firstIndex, args->vertexOffset,
                       args->firstInstance);
      break;
    }
    case EndRendering: {
      const auto* args = reinterpret_cast<const pd::EndRenderingArgs*>(&payload.args[0]);
      auto& frame = mFrames[frameIndex];
      vkCmdEndRendering(frame.mainCmdBuffer);
      VkImageMemoryBarrier2 barrierPresent{
          .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
          .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
          .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
          .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
          .dstAccessMask = 0,
          .oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
          .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
          .image = swapchainInfo.images[imageIndex],
          .subresourceRange{
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .levelCount = 1,
              .layerCount = 1,
          },
      };
      VkDependencyInfo barrierPresentDependencyInfo{
          .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
          .imageMemoryBarrierCount = 1,
          .pImageMemoryBarriers = &barrierPresent,
      };
      vkCmdPipelineBarrier2(frame.mainCmdBuffer, &barrierPresentDependencyInfo);
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
      vkCmdClearColorImage(frame.mainCmdBuffer, swapchainInfo.images[frameIndex], VK_IMAGE_LAYOUT_GENERAL, &color, 1,
                           &subResourceRange);
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