#include "pd/backend/vulkan/vulkan_command_manager.hpp"

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

VulkanCommandManager::VulkanCommandManager(VulkanResourceManager& rscMgr) noexcept
    : mCmdPool(rscMgr) {}

HwHandle<CommandRecorder_t> VulkanCommandManager::getCommandRecorder() noexcept {
  return mCmdPool.getIdleCmdBuffer();
}
}  // namespace pd