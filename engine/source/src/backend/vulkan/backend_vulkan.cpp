#include "pd/backend/vulkan/backend_vulkan.hpp"

// #include "pd/platform/rhi/vulkan/vulkan_texture.hpp"

namespace pd {
BackendVulkan::BackendVulkan(std::unique_ptr<VulkanContext>&& ctx) noexcept
    : mVulkanContext(std::move(ctx)),
      mVulkanResourceManager(mVulkanContext.get()) {
  //   for (auto& frame : mVulkanFrames) {
  //     frame.initialize(*mVulkanContext);
  //   }
}

BackendVulkan::~BackendVulkan() {
  // for (auto& frameData : mVulkanFrames) {
  //   frameData.shutdown();
  // }
}

HwHandle<Swapchain_t> BackendVulkan::createSwapchain(const HwSwapchain& hwSwapchain) noexcept {
  return mVulkanResourceManager.createSwapchain(hwSwapchain);
}

void BackendVulkan::destroySwapchain(const HwHandle<Swapchain_t>& handle) noexcept {
  mVulkanResourceManager.destroySwapchain(handle);
}

HwHandle<Buffer_t> BackendVulkan::createBuffer(const HwBuffer& buffer) noexcept {
  return mVulkanResourceManager.createBuffer(buffer);
}

void BackendVulkan::destroyBuffer(const HwHandle<Buffer_t>& handle) noexcept {
  mVulkanResourceManager.destroyBuffer(handle);
}

void BackendVulkan::writeBuffer(const BufferWriteOptions& writeOptions) noexcept {
  mVulkanResourceManager.writeBuffer(writeOptions);
}

Result<void> BackendVulkan::newFrame(HwHandle<Swapchain_t>& handle) noexcept {
  auto device = mVulkanContext->getDevice();
  // TODO(author): vulkan frame start
  return {};
}

Result<void> BackendVulkan::endFrame(HwHandle<Swapchain_t>& handle) noexcept {
  auto device = mVulkanContext->getDevice();
  // TODO(author): vulkan frame end
  return {};
}

/*
void BackendVulkan::acquireNextFrame() noexcept {
  auto device = mVulkanContext.getDevice();
  auto& currentFrame = getCurrentFrame();

  // fence previous frame util completed
  currentFrame.startFence();
  // acquire next swapchain image
  VkSemaphore imageAvailableSempahore = currentFrame.getImageAvailableSemaphore();
  bool acquireComplete = mVulkanSwapchain.acquireNextImage(imageAvailableSempahore);
  assert(acquireComplete);
}

void BackendVulkan::startCmdRecording() noexcept {
  auto& currentFrame = getCurrentFrame();
  auto currentCmdBuffer = currentFrame.getCommandBuffer();
  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };
  vkResetCommandBuffer(currentCmdBuffer, 0);

  auto result = vkBeginCommandBuffer(currentCmdBuffer, &beginInfo);
  PD_ASSERT_MSG(result == VK_SUCCESS, string_VkResult(result));
}

void BackendVulkan::endCmdRecording() noexcept {
  auto& currentFrame = getCurrentFrame();
  auto currentCmdBuffer = currentFrame.getCommandBuffer();

  vkEndCommandBuffer(currentCmdBuffer);
}

void BackendVulkan::submitFrame() noexcept {
  auto device = mVulkanContext.getDevice();
  auto& currentFrame = getCurrentFrame();
  auto currentCmdBuffer = currentFrame.getCommandBuffer();
  VkFence frameFence = currentFrame.getFrameFence();

  std::array<VkSemaphore, 1> waitSemaphores = {currentFrame.getImageAvailableSemaphore()};
  std::array<VkSemaphore, 1> signalSemaphores = {
      mVulkanSwapchain.getCurrentRenderFinishedSemaphore()};
  std::array<VkPipelineStageFlags, 1> waitStages = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSubmitInfo submitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount =
          waitSemaphores.size(),  // 在command buffer执行前要等待的信号量数量
      .pWaitSemaphores = waitSemaphores.data(),  // 执行前要等待的信号量
      .pWaitDstStageMask =
          waitStages.data(),    // 在哪几个pipeline的stage要等待这些wait semaphores
      .commandBufferCount = 1,  // 有几个command buffer
      .pCommandBuffers = &currentCmdBuffer,  // command buffers
      .signalSemaphoreCount =
          signalSemaphores
              .size(),  // 当指令在command buffer中执行完时要通知的semaphores数量
      .pSignalSemaphores = signalSemaphores.data(),  // 要通知的semaphores
  };
  // reset current frame's fence
  currentFrame.resetFence();
  auto result = vkQueueSubmit(mVulkanContext.getQueueInfo().presentQueue, 1, &submitInfo,
                              frameFence);
  PD_ASSERT_MSG(result == VK_SUCCESS, "vulkan queue submit error!");
}

void RhiVulkan::presentFrame() noexcept { mVulkanSwapchain.present(); }

void RhiVulkan::endFrame() noexcept {
  mCurrentFrameIndex = (mCurrentFrameIndex + 1) % INFLIGHT_FRAME_COUNT;

  // TODO(author): gc()
}

RhiHandle<RhiTexture> BackendVulkan::createTexture(uint32_t width, uint32_t height,
                                                   uint32_t depth) noexcept {
  return mResourceManager.createTexture(width, height, depth);
}

void BackendVulkan::updateTexture(RhiHandle<RhiTexture> handle,
                                  void* pPixelData) noexcept {
  // copy data
  // update texture image
}

void BackendVulkan::destroyTexture(RhiHandle<RhiTexture> handle) noexcept {
  if (!handle) {
    return;
  }
  // TODO(author): destroy vulkan texture
  return;
}
  */
}  // namespace pd