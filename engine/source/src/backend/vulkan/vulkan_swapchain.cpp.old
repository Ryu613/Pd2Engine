#include "pd/backend/vulkan/vulkan_swapchain.hpp"

#include "VkBootstrap.h"

#include "pd/backend/vulkan/vulkan_context.hpp"

namespace pd {
VulkanSwapchain::VulkanSwapchain(VulkanContext* ctx) noexcept
    : mContext(ctx) {}

VulkanSwapchain::~VulkanSwapchain() { destroy(); }

void VulkanSwapchain::destroy() noexcept {
  log::info("destroying vulkan swapchain...");
  destroySwapchain();
}

bool VulkanSwapchain::createSwapchain(uint32_t width, uint32_t height) noexcept {
  auto* physicalDevice = mContext->getPhysicalDevice();
  auto* device = mContext->getDevice();
  auto* surface = mContext->getSurface();
  const auto queueInfo = mContext->getQueueInfo();

  VkSurfaceCapabilitiesKHR surfaceCaps{};
  VK_CHECK(
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps));

  // ref(google filament)
  // one more than minimum swapchain length to reduce frame's buffer's waiting time
  // except the situation of using MAILBOX and allow tearing
  uint32_t desiredImageCount = surfaceCaps.minImageCount + 1;
  // maxImageCount = 0 means no limit on number of images, but, in fact, is limited by
  // total amount of memory used by presentable images.
  if (surfaceCaps.maxImageCount != 0 && desiredImageCount > surfaceCaps.maxImageCount) {
    log::error("swapchain doesn't support {} images.", desiredImageCount);
    desiredImageCount = surfaceCaps.minImageCount;
  }

  vkb::SwapchainBuilder swapChainBuilder(physicalDevice, device, surface,
                                         queueInfo.graphicsQueueFamilyIndex,
                                         queueInfo.presentQueueFamilyIndex);
  if (mSwapChain != VK_NULL_HANDLE) {
    swapChainBuilder.set_old_swapchain(mSwapChain);
  }
  auto ret =
      swapChainBuilder
          .set_desired_format(VkSurfaceFormatKHR{
              .format = mSwapChainInfo.imageFormat,
              .colorSpace = mSwapChainInfo.colorSpace,
          })
          .set_desired_min_image_count(desiredImageCount)
          .set_desired_present_mode(mSwapChainInfo.presentMode)
          .set_desired_extent(width, height)
          .add_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                 //| VK_IMAGE_USAGE_TRANSFER_DST_BIT // allows copy frame
                                 //| VK_IMAGE_USAGE_TRANSFER_SRC_BIT // allows read pixels
                                 )
          .build();
  if (!ret) {
    log::error(ret.error().message() + " " + string_VkResult(ret.vk_result()) + "\n");
    return false;
  }
  auto& swapchain = ret.value();
  mSwapChain = swapchain.swapchain;
  // refill swap chain info with result
  mSwapChainInfo.extent = swapchain.extent;
  mSwapChainInfo.imageFormat = swapchain.image_format;
  mSwapChainInfo.colorSpace = swapchain.color_space;
  mSwapChainInfo.presentMode = swapchain.present_mode;
  mSwapChainInfo.images = swapchain.get_images().value();
  mSwapChainInfo.imageViews = swapchain.get_image_views().value();

  // create sync objects
  mSwapChainInfo.renderFinishedSemaphore.resize(mSwapChainInfo.images.size());
  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };
  for (size_t i = 0; i < mSwapChainInfo.images.size(); ++i) {
    VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                               &mSwapChainInfo.renderFinishedSemaphore[i]));
  }

  // create depth images
  std::vector<VkFormat> depthFormatList{VK_FORMAT_D32_SFLOAT_S8_UINT,
                                        VK_FORMAT_D24_UNORM_S8_UINT};
  VkFormat depthFormat{VK_FORMAT_UNDEFINED};
  for (VkFormat& format : depthFormatList) {
    VkFormatProperties2 formatProperties{.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
    vkGetPhysicalDeviceFormatProperties2(physicalDevice, format, &formatProperties);
    // optimalTilingFeatures: makes sure the image is stored in a format best suited for
    // the GPU VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: use it as the depth attachment
    // for our render output
    if (formatProperties.formatProperties.optimalTilingFeatures &
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
      depthFormat = format;
      break;
    }
  }
  PD_ASSERT(depthFormat != VK_FORMAT_UNDEFINED);

  VkImageCreateInfo depthImageCI{
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = depthFormat,
      .extent{.width = width, .height = height, .depth = 1},
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
  VkImageViewCreateInfo depthViewCI{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = depthFormat,
      .subresourceRange{
          .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .levelCount = 1, .layerCount = 1}};
  VmaAllocationCreateInfo allocCI{.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                                  .usage = VMA_MEMORY_USAGE_AUTO};
  auto* vmaAllocator = mContext->getVmaAllocator();
  // resize to swapchain images size
  mSwapChainInfo.depthImages.resize(mSwapChainInfo.images.size());
  mSwapChainInfo.depthImageViews.resize(mSwapChainInfo.images.size());
  mSwapChainInfo.depthImageAllocation.resize(mSwapChainInfo.images.size());
  // means each swapchain image has its own depth image
  for (size_t i = 0; i < mSwapChainInfo.images.size(); ++i) {
    VK_CHECK(vmaCreateImage(vmaAllocator, &depthImageCI, &allocCI,
                            &mSwapChainInfo.depthImages[i],
                            &mSwapChainInfo.depthImageAllocation[i], nullptr));
    depthViewCI.image = mSwapChainInfo.depthImages[i];
    VK_CHECK(vkCreateImageView(device, &depthViewCI, nullptr,
                               &mSwapChainInfo.depthImageViews[i]));
  }

  return true;
}

void VulkanSwapchain::destroySwapchain() noexcept {
  auto* device = mContext->getDevice();
  auto* vmaAllocator = mContext->getVmaAllocator();
  if (!mSwapChainInfo.depthImageViews.empty()) {
    for (auto* depthView : mSwapChainInfo.depthImageViews) {
      vkDestroyImageView(device, depthView, nullptr);
    }
  }
  if (!mSwapChainInfo.renderFinishedSemaphore.empty()) {
    for (auto* semaphore : mSwapChainInfo.renderFinishedSemaphore) {
      vkDestroySemaphore(device, semaphore, nullptr);
    }
  }

  if (!mSwapChainInfo.depthImages.empty()) {
    for (size_t i = 0; i < mSwapChainInfo.depthImages.size(); ++i) {
      vmaDestroyImage(vmaAllocator, mSwapChainInfo.depthImages[i],
                      mSwapChainInfo.depthImageAllocation[i]);
    }
  }
  if (!mSwapChainInfo.imageViews.empty()) {
    for (auto* view : mSwapChainInfo.imageViews) {
      vkDestroyImageView(device, view, nullptr);
    }
  }

  if (mSwapChain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(device, mSwapChain, nullptr);
    mSwapChain = VK_NULL_HANDLE;
  }

  mSwapChainInfo = SwapChainInfo{};
}

// bool VulkanSwapchain::acquireNextImage(VkSemaphore imageAvailableSemaphore) noexcept {
//   auto device = mContext.getDevice();

//   auto result =
//       vkAcquireNextImageKHR(device, mSwapChain, UINT64_MAX, imageAvailableSemaphore,
//                             VK_NULL_HANDLE, &mCurrentImageIndex);
//   // if window resized
//   if (result == VK_ERROR_OUT_OF_DATE_KHR) {
//     // TODO(author): recreate swapchain
//   } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
//     log::error("failed to acquire swapchain image,err: \n{}", string_VkResult(result));
//     return false;
//   }
//   return true;
// }

// void VulkanSwapchain::present() noexcept {
//   auto presentQueue = mContext.getQueueInfo().presentQueue;
//   std::array<VkSemaphore, 1> signalSemaphores = {getCurrentRenderFinishedSemaphore()};
//   uint32_t currentImageIndex = getCurrentImageIndex();
//   VkPresentInfoKHR presentInfo{
//       .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
//       .waitSemaphoreCount = 1,
//       .pWaitSemaphores =
//           signalSemaphores.data(),  // 此处即在呈现前要等待image finished semaphore通知
//       .swapchainCount = 1,          // 要在哪些swapchain上呈现
//       .pSwapchains = &mSwapChain,
//       .pImageIndices = &currentImageIndex,  // 要呈现在swapchain的哪个索引对应的image
//   };
//   // 执行呈现
//   auto presentResult = vkQueuePresentKHR(presentQueue, &presentInfo);
//   if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
//   {
//     // TODO(author): 处理交换链过期问题，即需要重建swapchain
//   } else if (presentResult != VK_SUCCESS) {
//     log::error("failed to present swapchain image!\n{}",
//     string_VkResult(presentResult)); return;
//   }
// }
}  // namespace pd