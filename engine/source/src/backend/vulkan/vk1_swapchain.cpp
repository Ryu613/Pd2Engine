#include "vk1_swapchain.hpp"

#include "vk1_device.hpp"
#include "vk1_helper.hpp"

namespace vk1 {
Vk1Swapchain::Vk1Swapchain(Vk1Device& device)
    : mDevice(device) {}

Vk1Swapchain::~Vk1Swapchain() { destroy(); }

void Vk1Swapchain::create(uint32_t width, uint32_t height) {
  initSwapchainInfo(width, height);
  auto surface = mDevice.getSurface();
  auto& queueInfo = mDevice.getQueueInfo();
  // TODO(author): no resize()
  // shortcut
  auto device = mDevice.getDevice();
  auto minImageCount = std::clamp(2U,                               //
                                  mInfo.surfaceCaps.minImageCount,  //
                                  (mInfo.surfaceCaps.maxImageCount != 0) ? mInfo.surfaceCaps.maxImageCount : 2U);
  VkSwapchainCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = minImageCount,
      .imageFormat = mInfo.format,
      .imageColorSpace = mInfo.colorSpace,
      .imageExtent =
          {
              .width = width,
              .height = height,
          },
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
      .queueFamilyIndexCount = 1,
      .pQueueFamilyIndices = &queueInfo.presentQueueFamilyIndex,
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
  mInfo.presentSemaphores.resize(mInfo.images.size());
  auto subResourceRange = helper::createImageSubresourceRange();
  for (size_t i = 0; i < mInfo.images.size(); ++i) {
    mInfo.imageViews[i] = mDevice.createImageView(mInfo.images[i], mInfo.format, subResourceRange);
    // init image state
    mInfo.imageStates[i] = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };
    // create present semaphore
    mInfo.presentSemaphores[i] = mDevice.createSemaphore();
    assert(mInfo.presentSemaphores[i]);
  }
}

void Vk1Swapchain::destroy() {
  if (mSwapchain == VK_NULL_HANDLE) {
    return;
  }
  auto vkDevice = mDevice.getDevice();
  vkDeviceWaitIdle(vkDevice);

  if (!mInfo.presentSemaphores.empty()) {
    for (auto presentSemaphore : mInfo.presentSemaphores) {
      mDevice.destroySemaphore(presentSemaphore);
    }
  }
  if (!mInfo.imageViews.empty()) {
    for (auto imageView : mInfo.imageViews) {
      vkDestroyImageView(vkDevice, imageView, 0);
    }
  }
  vkDestroySwapchainKHR(vkDevice, mSwapchain, 0);
  mInfo = {};
}

uint32_t Vk1Swapchain::acquireNextImage(VkSemaphore semaphore) {
  uint32_t imageIndex = 0;
  vkAcquireNextImageKHR(mDevice.getDevice(), mSwapchain, UINT64_MAX, semaphore, 0, &imageIndex);
  return imageIndex;
}
void Vk1Swapchain::present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
  const auto& queueInfo = mDevice.getQueueInfo();
  VkPresentInfoKHR presentInfo{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &waitSemaphore,
      .swapchainCount = 1,
      .pSwapchains = &mSwapchain,
      .pImageIndices = &imageIndex,
  };
  checkResult(vkQueuePresentKHR(queueInfo.presentQueue, &presentInfo));
}

void Vk1Swapchain::initSwapchainInfo(uint32_t width, uint32_t height) {
  assert(width > 0 && height > 0);
  auto physicalDevice = mDevice.getPhysicalDevice();
  auto surface = mDevice.getSurface();
  VkSurfaceCapabilitiesKHR surfaceCaps{};
  checkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps));
  assert(surfaceCaps.currentExtent.width == 0xFFFFFFFF || width == surfaceCaps.currentExtent.width);

  mInfo.extent.width = width;
  mInfo.extent.height = height;
  mInfo.surfaceCaps = surfaceCaps;

  auto surfaceFormats = getSupportedSurfaceFormats();
  assert(!surfaceFormats.empty());
  for (uint32_t i = 0; i < surfaceFormats.size(); ++i) {
    if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB) {
      mInfo.format = surfaceFormats[i].format;
      mInfo.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
      break;
    }
  }
  assert(mInfo.format != VK_FORMAT_UNDEFINED);
  assert(mInfo.colorSpace != VK_COLOR_SPACE_MAX_ENUM_KHR);

  auto presentModes = getSupportedPresentModes();
  assert(!presentModes.empty());
  for (uint32_t i = 0; i < presentModes.size(); ++i) {
    if (presentModes[i] == VK_PRESENT_MODE_FIFO_KHR) {
      mInfo.presentMode = presentModes[i];
      break;
    }
  }
  assert(mInfo.presentMode != VK_PRESENT_MODE_MAX_ENUM_KHR);
}

std::vector<VkSurfaceFormatKHR> Vk1Swapchain::getSupportedSurfaceFormats() {
  auto physicalDevice = mDevice.getPhysicalDevice();
  auto surface = mDevice.getSurface();
  uint32_t formatCount = 0;
  checkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, 0));
  assert(formatCount > 0);
  std::vector<VkSurfaceFormatKHR> formats{formatCount};
  checkResult(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data()));
  return formats;
}

std::vector<VkPresentModeKHR> Vk1Swapchain::getSupportedPresentModes() {
  auto physicalDevice = mDevice.getPhysicalDevice();
  auto surface = mDevice.getSurface();
  uint32_t presentModeCount = 0;
  checkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, 0));
  assert(presentModeCount > 0);
  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  checkResult(
      vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));
  return presentModes;
}
}  // namespace vk1