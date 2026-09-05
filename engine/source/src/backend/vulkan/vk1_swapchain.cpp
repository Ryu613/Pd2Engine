#include "vk1_swapchain.hpp"

#include "vk1_device.hpp"
#include "vk1_helper.hpp"

namespace vk1 {
Vk1Swapchain::Vk1Swapchain(Vk1Device& device)
    : mpDevice(&device) {}

Vk1Swapchain::~Vk1Swapchain() {}

void Vk1Swapchain::create(VkSurfaceKHR surface, uint32_t queueFamilyIndex, uint32_t width,
                          uint32_t height) {
  // TODO(author): no resize()
  // shortcut
  mInfo.extent.width = width;
  mInfo.extent.height = height;
  auto device = mpDevice->getDevice();
  VkFormat defaultFormat = VK_FORMAT_R8G8B8A8_SRGB;
  mInfo.format = defaultFormat;
  VkSwapchainCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = 2,
      .imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
      .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
      .imageExtent =
          {
              .width = width,
              .height = height,
          },
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .queueFamilyIndexCount = 1,
      .pQueueFamilyIndices = &queueFamilyIndex,
      .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
  };
  checkResult(vkCreateSwapchainKHR(device, &createInfo, nullptr, &mSwapchain));
  assert(mSwapchain);
  uint32_t count = 0;
  checkResult(vkGetSwapchainImagesKHR(device, mSwapchain, &count, nullptr));
  assert(count != 0);
  mInfo.images.resize(count);
  vkGetSwapchainImagesKHR(device, mSwapchain, &count, mInfo.images.data());

  // create swapchain image views
  mInfo.imageViews.resize(mInfo.images.size());
  mInfo.imageStates.resize(mInfo.images.size());
  auto subResourceRange = helper::createImageSubresourceRange();
  for (size_t i = 0; i < mInfo.images.size(); ++i) {
    mInfo.imageViews[i] =
        mpDevice->createImageView(mInfo.images[i], defaultFormat, subResourceRange);
    // init image state
    mInfo.imageStates[i] = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };
  }
}

void Vk1Swapchain::destroy() {
  if (!mSwapchain) {
    return;
  }
  if (!mInfo.imageViews.empty()) {
    for (auto imageView : mInfo.imageViews) {
      vkDestroyImageView(mpDevice->getDevice(), imageView, 0);
    }
  }
  vkDestroySwapchainKHR(mpDevice->getDevice(), mSwapchain, 0);
  mInfo = {};
}

uint32_t Vk1Swapchain::acquireNextImage(VkSemaphore semaphore) {
  uint32_t imageIndex = 0;
  vkAcquireNextImageKHR(mpDevice->getDevice(), mSwapchain, UINT64_MAX, semaphore, 0, &imageIndex);
  return imageIndex;
}
void Vk1Swapchain::present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
  VkPresentInfoKHR presentInfo{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &waitSemaphore,
      .swapchainCount = 1,
      .pSwapchains = &mSwapchain,
      .pImageIndices = &imageIndex,
  };
  vkQueuePresentKHR(mpDevice->getQueue(), &presentInfo);
}
}  // namespace vk1