#include "pd/backend/vulkan/resource/vulkan_resource_manager.hpp"

#include "pd/backend/vulkan/vulkan_types.hpp"
#include "pd/backend/vulkan/vulkan_context.hpp"
#include "VkBootstrap.h"

namespace pd {
template <typename T>
using Handle = VulkanResourceManager::Handle<T>;

VulkanResourceManager::VulkanResourceManager(VulkanContext* ctx) noexcept
    : mVulkanContext(ctx) {}

Handle<Texture_t> VulkanResourceManager::createImage(const HwTexture& texture) noexcept {
  auto* device = mVulkanContext->getDevice();

  VkImageCreateInfo imageCInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .flags = texture.isProtected ? VK_IMAGE_CREATE_PROTECTED_BIT : 0u,
      .imageType = ToVkImageType(texture.type),
      .format = ToVkFormat(texture.format),
      .extent =
          {
              .width = texture.extent.width,
              .height = texture.extent.height,
              .depth = texture.extent.depth,
          },
      .mipLevels = texture.mipLevel,
      .arrayLayers = texture.arrayLeyers,
      .samples = ToVkSampleFlagBits(texture.samples),
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = GetVkImageUsageFlagBits(texture.usage),
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      // queue family index
      .initialLayout = ToVkImageLayout(texture.initialLayout),
  };

  if (texture.type == TextureType::TextureTypeCube) {
    imageCInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
  }

  VmaAllocationCreateInfo allocCInfo{
      .usage = VMA_MEMORY_USAGE_AUTO,
  };

  // TODO(author): external image?

  auto* allocator = mVulkanContext->getVmaAllocator();
  VkImage vkImage{};
  VmaAllocation vmaAllocation{};
  if (auto result = vmaCreateImage(allocator, &imageCInfo, &allocCInfo, &vkImage,
                                   &vmaAllocation, nullptr);
      result != VK_SUCCESS) {
    log::error("vulkan create image error: {}", string_VkResult(result));
    return {};
  }
  VmaAllocationInfo allocInfo;
  vmaGetAllocationInfo(allocator, vmaAllocation, &allocInfo);

  setObjectName(device, VK_OBJECT_TYPE_IMAGE, vkImage, texture.label);

  const auto textureHandle =
      mTextures.emplace(VulkanImage{texture, this, vkImage, vmaAllocation});

  return textureHandle;
}

void VulkanResourceManager::destroyImage(const Handle<Texture_t>& handle) noexcept {
  auto* vulkanImage = mTextures.get(handle);
  auto* allocator = mVulkanContext->getVmaAllocator();
  vmaDestroyImage(allocator, vulkanImage->handle, vulkanImage->allocation);

  mTextures.remove(handle);
}

[[nodiscard]] VulkanImage* VulkanResourceManager::getImage(
    const Handle<Texture_t>& handle) const noexcept {
  return mTextures.get(handle);
}

Handle<Swapchain_t> VulkanResourceManager::createSwapchain(
    const HwSwapchain& swapchain) noexcept {
  auto* physicalDevice = mVulkanContext->getPhysicalDevice();
  auto* device = mVulkanContext->getDevice();
  auto* surface = mVulkanContext->getSurface();
  const auto queueInfo = mVulkanContext->getQueueInfo();

  VkSurfaceCapabilitiesKHR surfaceCaps{};
  auto deviceSurfaceResult =
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps);
  PD_ASSERT_MSG(deviceSurfaceResult == VK_SUCCESS, "get vulkan device surface error");
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
  if (swapchain.oldSwapchain.isValid()) {
    swapChainBuilder.set_old_swapchain(getSwapchain(swapchain.oldSwapchain)->handle);
  }
  auto ret =
      swapChainBuilder
          .set_desired_format(VkSurfaceFormatKHR{
              .format = ToVkFormat(swapchain.format),
              .colorSpace = ToVkColorSpaceKHR(swapchain.colorSpace),
          })
          .set_desired_min_image_count(desiredImageCount)
          .set_desired_present_mode(ToVkPresentMode(swapchain.presentMode))
          .set_desired_extent(swapchain.extent.width, swapchain.extent.height)
          .add_image_usage_flags(GetVkImageUsageFlagBits(swapchain.imageUsageFlags)
                                 //| VK_IMAGE_USAGE_TRANSFER_DST_BIT // allows copy frame
                                 //| VK_IMAGE_USAGE_TRANSFER_SRC_BIT // allows read pixels
                                 )
          .build();
  if (!ret) {
    log::error(ret.error().message() + " " + string_VkResult(ret.vk_result()) + "\n");
    PD_ASSERT_MSG(deviceSurfaceResult == VK_SUCCESS, "build vulkan swapchain error");
  }
  auto& newSwapchain = ret.value();

  // 参数以实际创建出来的为准
  SwapChainInfo swapchainInfo{
      .imageFormat = newSwapchain.image_format,
      .colorSpace = newSwapchain.color_space,
      .presentMode = newSwapchain.present_mode,
      .extent = newSwapchain.extent,
      .images = newSwapchain.get_images().value(),
      .imageViews = newSwapchain.get_image_views().value(),
  };
  // create sync objects
  swapchainInfo.renderFinishedSemaphore.resize(swapchainInfo.images.size());
  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };
  for (size_t i = 0; i < swapchainInfo.images.size(); ++i) {
    VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                               &swapchainInfo.renderFinishedSemaphore[i]));
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
      .extent{
          .width = newSwapchain.extent.width,
          .height = newSwapchain.extent.height,
          .depth = 1,
      },
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
  VkImageViewCreateInfo depthViewCI{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                    .format = depthFormat,
                                    .subresourceRange{
                                        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                                        .levelCount = 1,
                                        .layerCount = 1,
                                    }};
  VmaAllocationCreateInfo allocCI{.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
                                  .usage = VMA_MEMORY_USAGE_AUTO};
  auto* vmaAllocator = mVulkanContext->getVmaAllocator();
  // resize to swapchain images size
  swapchainInfo.depthImages.resize(swapchainInfo.images.size());
  swapchainInfo.depthImageViews.resize(swapchainInfo.images.size());
  swapchainInfo.depthImageAllocation.resize(swapchainInfo.images.size());
  // means each swapchain image has its own depth image
  for (size_t i = 0; i < swapchainInfo.images.size(); ++i) {
    VK_CHECK(vmaCreateImage(vmaAllocator, &depthImageCI, &allocCI,
                            &swapchainInfo.depthImages[i],
                            &swapchainInfo.depthImageAllocation[i], nullptr));
    depthViewCI.image = swapchainInfo.depthImages[i];
    VK_CHECK(vkCreateImageView(device, &depthViewCI, nullptr,
                               &swapchainInfo.depthImageViews[i]));
  }

  setObjectName(device, VK_OBJECT_TYPE_SWAPCHAIN_KHR, newSwapchain.swapchain,
                swapchain.label);
  const auto swapchainHandle = mSwapchains.emplace(
      VulkanSwapchain{swapchain, this, newSwapchain.swapchain, std::move(swapchainInfo)});
  return swapchainHandle;
}
void VulkanResourceManager::destroySwapchain(const Handle<Swapchain_t>& handle) noexcept {
  if (!handle.isValid()) {
    return;
  }
  auto* vulkanSwapchain = mSwapchains.get(handle);
  auto* device = mVulkanContext->getDevice();
  auto* vmaAllocator = mVulkanContext->getVmaAllocator();
  auto& info = vulkanSwapchain->swapchainInfo;
  if (!info.depthImageViews.empty()) {
    for (auto* depthView : info.depthImageViews) {
      vkDestroyImageView(device, depthView, nullptr);
    }
  }
  if (!info.renderFinishedSemaphore.empty()) {
    for (auto* semaphore : info.renderFinishedSemaphore) {
      vkDestroySemaphore(device, semaphore, nullptr);
    }
  }

  if (!info.depthImages.empty()) {
    for (size_t i = 0; i < info.depthImages.size(); ++i) {
      vmaDestroyImage(vmaAllocator, info.depthImages[i], info.depthImageAllocation[i]);
    }
  }
  if (!info.imageViews.empty()) {
    for (auto* view : info.imageViews) {
      vkDestroyImageView(device, view, nullptr);
    }
  }

  if (vulkanSwapchain->handle != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(device, vulkanSwapchain->handle, nullptr);
    vulkanSwapchain->handle = VK_NULL_HANDLE;
  }

  vkDestroySwapchainKHR(device, vulkanSwapchain->handle, nullptr);

  mSwapchains.remove(handle);
}
[[nodiscard]] VulkanSwapchain* VulkanResourceManager::getSwapchain(
    const Handle<Swapchain_t>& handle) const noexcept {
  return mSwapchains.get(handle);
}

HwHandle<Buffer_t> VulkanResourceManager::createBuffer(const HwBuffer& buffer) noexcept {
  auto device = mVulkanContext->getDevice();

  VkBufferCreateInfo cinfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = buffer.deviceSize,
      .usage = ToVkBufferUsage(buffer.usage),
      .sharingMode = ToVkSharingMode(buffer.sharingMode),
  };
  VmaAllocationCreateInfo allocInfo{
      .usage = ToVkMemoryUsage(buffer.memoryUsage),
  };
  auto allocator = mVulkanContext->getVmaAllocator();
  // external memory buffer ?

  VkBuffer vkBuffer;
  VmaAllocation allocation;

  auto createResult =
      vmaCreateBuffer(allocator, &cinfo, &allocInfo, &vkBuffer, &allocation, nullptr);
  PD_ASSERT_MSG(createResult == VK_SUCCESS, "buffer create failed!");

  setObjectName(device, VK_OBJECT_TYPE_BUFFER, vkBuffer, buffer.label);
  const auto bufferHandle =
      mBuffers.emplace(VulkanBuffer{buffer, this, vkBuffer, std::move(allocation)});
  return bufferHandle;

  return {};
}

void VulkanResourceManager::destroyBuffer(const HwHandle<Buffer_t>& handle) noexcept {
  auto* vulkanBuffer = mBuffers.get(handle);
  auto* vmaAllocator = mVulkanContext->getVmaAllocator();

  vmaDestroyBuffer(vmaAllocator, vulkanBuffer->handle, vulkanBuffer->allocation);

  mBuffers.remove(handle);
}
}  // namespace pd