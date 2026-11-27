#include "staging_manager.hpp"

#include "../vk1_device.hpp"

namespace vk1 {
StagingManager::StagingManager(Vk1Device& device, ResourceRegistry& registry)
    : mDevice(device),
      mRegistry(registry),
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

  mStagingBuffer = mDevice.createBuffer(global::defaultStagingBufferSize,             // buffer size
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,             // buffer usage
                                        VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT,  // vma flag
                                        VMA_MEMORY_USAGE_CPU_TO_GPU                   // vma usage
  );
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

  mDevice.waitFences(mFence);
  // growable
  if (data.size() > mStagingBuffer.size /* && data.size() <= physical device max supported size*/) {
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

  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };
  vkBeginCommandBuffer(mCmdBuffer, &beginInfo);

  // add barrier for buffer to image copy
  // todo: resource status tracking for automating insert barriers

  // transition states
  const ImageState transferDst{
      .imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .pipelineStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
      .accessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
  };

  mDevice.addImageBarrier(image, mCmdBuffer, image.states[0], transferDst);
  // copy buffer to image
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

  VkCopyBufferToImageInfo2 copyInfo{
      .srcBuffer = mStagingBuffer.handle,
      .dstImage = image.handle,
      .dstImageLayout = transferDst.imageLayout,
      .regionCount = 1,
      .pRegions = &region,
  };

  vkCmdCopyBufferToImage2(mCmdBuffer, &copyInfo);
  // update image state
  image.states[0] = transferDst;

  // generate mipmap, but first transfer image to src to let mipmap can be blited from
  if (image.mipLevels > 1) {
    assert(false);
    const ImageState transferSrc{
        .imageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .pipelineStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        .accessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };
    mDevice.addImageBarrier(image, mCmdBuffer, image.states[0], transferSrc);
    image.states[0] = transferSrc;

    // todo
  }

  const ImageState shaderRead{
      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      .pipelineStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
      .accessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
  };
  mDevice.addImageBarrier(image, mCmdBuffer, image.states[0], shaderRead);
  image.states[0] = shaderRead;

  vkEndCommandBuffer(mCmdBuffer);

  // submit
  VkCommandBufferSubmitInfo cmdInfo{
      .commandBuffer = mCmdBuffer,
  };
  VkSubmitInfo2 submitInfo{
      .commandBufferInfoCount = 1,
      .pCommandBufferInfos = &cmdInfo,
  };
  vkQueueSubmit2(mDevice.getQueueInfo().graphicsQueue, 1, &submitInfo, mFence);

  vkResetCommandBuffer(mCmdBuffer, 0);
}
}  // namespace vk1