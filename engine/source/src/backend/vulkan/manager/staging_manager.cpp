#include "staging_manager.hpp"

#include "../vk1_device.hpp"

namespace vk1 {
StagingManager::StagingManager(Vk1Device& device, ResourceRegistry& registry)
    : mDevice(device),
      mRegistry(registry),
      mMinBufferSize(kMinBufferSize),
      mMaxBufferSize(kMaxBufferSize) {
  init();
}
StagingManager::~StagingManager() { destroy(); }

void StagingManager::init() noexcept {
  mFence = mDevice.createFence();
  // use graphics queue for now!
  mCmdPool = mDevice.createCommandPool();
  VkCommandBufferAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = mCmdPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };
  checkResult(vkAllocateCommandBuffers(mDevice.getDevice(), &allocInfo, &mCmdBuffer));
}

void StagingManager::destroy() noexcept {
  if (mCmdPool) {
    mCmdBuffer = VK_NULL_HANDLE;
    vkDestroyCommandPool(mDevice.getDevice(), mCmdPool, 0);
  }
  if (mStagingBuffer.handle) {
    mDevice.destroyBuffer(mStagingBuffer);
  }
  if (mFence) {
    mDevice.destroyFence(mFence);
  }
}

void StagingManager::uploadImage(pd::HwTextureHandle handle, std::span<const std::byte> data) noexcept {
  if (!handle) {
    return;
  }
  assert(!data.empty());
  assert(data.size() <= mMaxBufferSize);

  auto* imagePtr = mRegistry.getResource(handle);
  if (!imagePtr) {
    return;
  }
  auto& image = *imagePtr;
  if (data.size() > mStagingBuffer.size) {
    mDevice.waitFences(mFence);
    mDevice.destroyBuffer(mStagingBuffer);
    mStagingBuffer = mDevice.createBuffer(data.size(),                                  // buffer size
                                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,             // buffer usage
                                          VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT,  // vma flag
                                          VMA_MEMORY_USAGE_CPU_TO_GPU                   // vma usage
    );
  }
  // copy data to buffer
  auto allocator = mDevice.getAllocator();
  void* dstData;
  vmaMapMemory(allocator, mStagingBuffer.allocation, &dstData);
  std::memcpy(dstData, data.data(), data.size());
  vmaUnmapMemory(allocator, mStagingBuffer.allocation);

  // transition states
  const ImageState transferDst{
      .imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .pipelineStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
      .accessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
  };
  const ImageState transferSrc{};
  const ImageState shaderRead{};

  // image transition mip 0
  image.transition(mCmdBuffer, transferDst);

  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };
  vkBeginCommandBuffer(mCmdBuffer, &beginInfo);

  // copy region
  VkBufferImageCopy2 region{
      .imageSubresource =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .imageExtent = image.extent,
  };

  // add barrier
  ImageState newSatate{};
  mDevice.addImageBarrier(image, mCmdBuffer, image.state, newSatate);

  mDevice.resetFences(mFence);
}
}  // namespace vk1