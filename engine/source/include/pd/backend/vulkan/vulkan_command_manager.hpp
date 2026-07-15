#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource_manager.hpp"

namespace pd {
class VulkanResourceManager;
class VulkanCommandBufferPool {
 public:
  explicit VulkanCommandBufferPool(VulkanResourceManager& rscMgr) noexcept;
  DELETE_COPY_MOVE(VulkanCommandBufferPool);
  ~VulkanCommandBufferPool();

  HwHandle<CommandRecorder_t> getIdleCmdBuffer() noexcept;

 private:
  struct CmdBufferSlot {
    HwHandle<CommandBuffer_t> handle;
    bool isRecording = false;
  };
  VulkanResourceManager* mResourceManager = nullptr;
  HwHandle<CommandPool_t> mPool;
  std::array<CmdBufferSlot, global::MaxCommandBuffers> mCmdBuffers;
};
class VulkanCommandManager {
 public:
  explicit VulkanCommandManager(VulkanResourceManager& rscMgr) noexcept;
  DELETE_COPY_MOVE(VulkanCommandManager);
  ~VulkanCommandManager() = default;

  HwHandle<CommandRecorder_t> getCommandRecorder() noexcept;

 private:
  VulkanCommandBufferPool mCmdPool;
};
}  // namespace pd