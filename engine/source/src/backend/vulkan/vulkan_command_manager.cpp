#include "pd/backend/vulkan/vulkan_command_manager.hpp"

#include "pd/backend/vulkan/vulkan_frame.hpp"

namespace pd {
VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanResourceManager& rscMgr) noexcept
    : mResourceManager(&rscMgr) {
  // create cmd pool
  mPool = rscMgr.createCommandPool();
  for (auto& cmdBuffer : mCmdBuffers) {
    cmdBuffer = {
        .handle = rscMgr.createCommandBuffer(mPool),
        .isRecording = false,
    };
  }
}

VulkanCommandBufferPool::~VulkanCommandBufferPool() {
  mResourceManager->destroyCommandPool(mPool);
}

HwHandle<CommandRecorder_t> VulkanCommandBufferPool::getIdleCmdBuffer() noexcept {
  for (auto& slot : mCmdBuffers) {
    if (!slot.isRecording) {
      slot.isRecording = true;
      return slot.handle;
    }
  }
  PD_ASSERT_MSG(false, "no valid vulkan command buffer exist");
  return {};
}

void VulkanCommandBufferPool::beginCmdBuffer(
    const HwHandle<CommandBuffer_t>& handle) noexcept {
  auto* cmdBuffer = mResourceManager->getCommandBuffer(handle);
  if (cmdBuffer) {
    cmdBuffer->begin();
  }
}

void VulkanCommandBufferPool::endCmdBuffer(const HwHandle<CommandBuffer_t>& handle) noexcept {
  auto* cmdBuffer = mResourceManager->getCommandBuffer(handle);
  if (!cmdBuffer) {
    return;
  }
  cmdBuffer->end();
  // may optimize this!
  for (auto& slot : mCmdBuffers) {
    if (slot.handle == handle) {
      slot.isRecording = false;
    }
  }
}

void VulkanCommandBufferPool::submitCmdBuffer(const HwHandle<CommandBuffer_t>& handle,
                                              std::span<VkSemaphore> waitSemaphore,
                                              std::span<VkSemaphore> signalSemaphore,
                                              VkFence fence) noexcept {
  auto* cmdBuffer = mResourceManager->getCommandBuffer(handle);
  if (!cmdBuffer) {
    return;
  }
  auto queue = mResourceManager->getContext().getQueueInfo().presentQueue;

  cmdBuffer->submit(queue, waitSemaphore, signalSemaphore, fence);
}

// --------------------VulkanCommandManager ---------------------------
// --------------------------------------------------------------------
VulkanCommandManager::VulkanCommandManager(VulkanResourceManager& rscMgr) noexcept
    : mCmdPool(rscMgr) {}

HwHandle<CommandRecorder_t> VulkanCommandManager::getCommandRecorder() noexcept {
  return mCmdPool.getIdleCmdBuffer();
}

void VulkanCommandManager::beginCommandRecord(
    const HwHandle<CommandRecorder_t>& handle) noexcept {
  mCmdPool.beginCmdBuffer(handle);
}
void VulkanCommandManager::endCommandRecord(
    const HwHandle<CommandRecorder_t>& handle) noexcept {
  mCmdPool.endCmdBuffer(handle);
}

void VulkanCommandManager::submitCommandRecord(const HwHandle<CommandRecorder_t>& handle,
                                               std::span<VkSemaphore> waitSemaphore,
                                               std::span<VkSemaphore> signalSemaphore,
                                               VkFence fence) noexcept {
  mCmdPool.submitCmdBuffer(handle, waitSemaphore, signalSemaphore, fence);
}
}  // namespace pd