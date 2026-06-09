#include "pd/platform/backend/vulkan/vulkan_driver.hpp"

namespace pd {
VulkanDriver::VulkanDriver(VulkanConfig config) noexcept
    : mConfig(std::move(config)),
      mVulkanSwapchain(mVulkanContext) {
  // init vulkan
  mVulkanContext.initialize(mConfig);
  PD_ASSERT_MSG(mVulkanContext.initialized(), "vulkan initialize failed!");
  // create swapchain
  mVulkanSwapchain.initialize(mConfig.windowWidth, mConfig.windowHeight);
  PD_ASSERT_MSG(mVulkanSwapchain.initialized(), "create vulkan swapchain failed!");
  // create frame data
  for (auto& frame : mVulkanFrames) {
    frame.initialize(mVulkanContext);
  }
}

VulkanDriver::~VulkanDriver() {
  // for (auto& frameData : mVulkanFrames) {
  //   frameData.shutdown();
  // }
}

void VulkanDriver::acquireNextFrame() noexcept {
  auto device = mVulkanContext.getDevice();
  auto& currentFrame = getCurrentFrame();

  // fence previous frame util completed
  currentFrame.startFence();
  // acquire next swapchain image
  VkSemaphore imageAvailableSempahore = currentFrame.getImageAvailableSemaphore();
  bool acquireComplete = mVulkanSwapchain.acquireNextImage(imageAvailableSempahore);
  assert(acquireComplete);
}

void VulkanDriver::startCmdRecording() noexcept {
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

void VulkanDriver::endCmdRecording() noexcept {
  auto& currentFrame = getCurrentFrame();
  auto currentCmdBuffer = currentFrame.getCommandBuffer();

  vkEndCommandBuffer(currentCmdBuffer);
}

void VulkanDriver::submitFrame() noexcept {
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

void VulkanDriver::presentFrame() noexcept { mVulkanSwapchain.present(); }

void VulkanDriver::endFrame() noexcept {
  mCurrentFrameIndex = (mCurrentFrameIndex + 1) % INFLIGHT_FRAME_COUNT;

  // TODO(author): gc()
}
}  // namespace pd