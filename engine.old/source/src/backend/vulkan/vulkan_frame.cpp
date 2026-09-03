#include "pd/backend/vulkan/vulkan_frame.hpp"

#include "pd/backend/vulkan/vulkan_context.hpp"

namespace pd {
VulkanFrame::~VulkanFrame() { shutdown(); }

VulkanFrame::VulkanFrame(VulkanFrame&& rhs) noexcept
    : mVulkanContext{std::exchange(rhs.mVulkanContext, nullptr)},
      //   mCommandPool{std::exchange(rhs.mCommandPool, VK_NULL_HANDLE)},
      //   mMainCommandBuffer{std::exchange(rhs.mMainCommandBuffer, VK_NULL_HANDLE)},
      mImageAvailableSemaphore{std::exchange(rhs.mImageAvailableSemaphore, VK_NULL_HANDLE)},
      mFrameFence{std::exchange(rhs.mFrameFence, VK_NULL_HANDLE)} {}

bool VulkanFrame::initialize(VulkanContext& context) noexcept {
  if (mInitialized) {
    return true;
  }
  mVulkanContext = &context;
  auto& ctx = *mVulkanContext;

  auto device = ctx.getDevice();
  auto queueFamilyIndex = ctx.getQueueInfo().graphicsQueueFamilyIndex;

  // create command pool & buffer
  //   VkCommandPoolCreateInfo cmdPoolInfo{
  //       .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
  //       .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
  //       .queueFamilyIndex = queueFamilyIndex,
  //   };

  //   VK_CHECK(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &mCommandPool));

  //   VkCommandBufferAllocateInfo cmdbufferAllocInfo{
  //       .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
  //       .commandPool = mCommandPool,
  //       .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
  //       .commandBufferCount = 1,
  //   };

  //   VK_CHECK(vkAllocateCommandBuffers(device, &cmdbufferAllocInfo, &mMainCommandBuffer));

  // create frame fence
  VkFenceCreateInfo fenceInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags =
          VK_FENCE_CREATE_SIGNALED_BIT,  // 创建时的fence状态，此处是已通知(waitFence时直接可pass)
  };
  VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &mFrameFence));

  // create semahores
  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };
  VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, 0, &mImageAvailableSemaphore));
  VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, 0, &mImagePresentSemaphore));

  mInitialized = true;
  return true;
}

void VulkanFrame::shutdown() noexcept {
  if (!mInitialized) {
    return;
  }
  auto device = mVulkanContext->getDevice();

  //   if (mCommandPool != nullptr) {
  //     vkDestroyCommandPool(device, mCommandPool, nullptr);
  //   }
  if (mFrameFence != nullptr) {
    vkDestroyFence(device, mFrameFence, nullptr);
  }

  vkDestroySemaphore(device, mImageAvailableSemaphore, 0);
  vkDestroySemaphore(device, mImagePresentSemaphore, 0);

  mVulkanContext = nullptr;

  mInitialized = false;
}

void VulkanFrame::startFence() noexcept {
  vkWaitForFences(mVulkanContext->getDevice(), 1, &mFrameFence, VK_TRUE, UINT64_MAX);
}

void VulkanFrame::resetFence() noexcept {
  vkResetFences(mVulkanContext->getDevice(), 1, &mFrameFence);
}
}  // namespace pd