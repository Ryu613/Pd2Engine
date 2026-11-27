#pragma once

#include "pd/backend/backend_types.hpp"
#include "resource_registry.hpp"

namespace vk1 {
class StagingManager {
 public:
  explicit StagingManager(Vk1Device& device, ResourceRegistry& registry);
  ~StagingManager();
  DELETE_COPY_MOVE(StagingManager);

  /**
   * @brief 图片上传
   *
   * @note 暂不支持生成mipmap
   * @todo staging buffer pool
   * @param image
   * @param data
   */
  void uploadImage(pd::HwTextureHandle imageHandle, std::span<const std::byte> data) noexcept;

 private:
  inline static constexpr uint32_t kMaxBufferSize = 128 * 1024 * 1024;
  inline static constexpr uint32_t kMinBufferSize = 16 * 1024 * 1024;

  struct BufferRegion {
    uint64_t offset = 0;
    uint64_t size = 0;
  };

  Vk1Device& mDevice;
  ResourceRegistry& mRegistry;
  Vk1Buffer mStagingBuffer;
  VkDeviceSize mMaxBufferSize;
  std::vector<BufferRegion> mRegions;
  VkFence mFence = VK_NULL_HANDLE;
  VkCommandPool mCmdPool = VK_NULL_HANDLE;
  VkCommandBuffer mCmdBuffer = VK_NULL_HANDLE;

  void init() noexcept;
  void destroy() noexcept;
};
}  // namespace vk1