#pragma once

#include "pd/backend/vulkan/vulkan_common.hpp"
#include "pd/backend/vulkan/resource/vulkan_resource_manager.hpp"

namespace pd {
class VulkanResourceManager;
class VulkanFrame;
class VulkanCommandBufferPool {
 public:
  explicit VulkanCommandBufferPool(VulkanResourceManager& rscMgr) noexcept;
  DELETE_COPY_MOVE(VulkanCommandBufferPool);
  ~VulkanCommandBufferPool();

  HwHandle<CommandRecorder_t> getIdleCmdBuffer() noexcept;
  void beginCmdBuffer(const HwHandle<CommandBuffer_t>& handle) noexcept;
  void endCmdBuffer(const HwHandle<CommandBuffer_t>& handle) noexcept;
  void submitCmdBuffer(const HwHandle<CommandBuffer_t>& handle,
                       std::span<VkSemaphore> waitSemaphore,
                       std::span<VkSemaphore> signalSemaphore, VkFence fence) noexcept;

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
  void beginCommandRecord(const HwHandle<CommandRecorder_t>& handle) noexcept;
  void endCommandRecord(const HwHandle<CommandRecorder_t>& handle) noexcept;
  void submitCommandRecord(const HwHandle<CommandRecorder_t>& handle,
                           std::span<VkSemaphore> waitSemaphore,
                           std::span<VkSemaphore> signalSemaphore, VkFence fence) noexcept;

 private:
  VulkanCommandBufferPool mCmdPool;
};
}  // namespace pd