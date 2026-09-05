#include "vk1_device.hpp"

#include "vk1_helper.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace vk1 {

Vk1Device::Vk1Device(Vk1Context&& ctx)
    : mContext(std::move(ctx)),
      mSwapchain(*this) {
  init();
}
Vk1Device::~Vk1Device() { destroy(); }

void Vk1Device::init() {
  // pick physical device
  pickPhysicalDevice();
  // create device
  VkPhysicalDeviceFeatures deviceFeatures{
      .samplerAnisotropy = VK_TRUE,  // better texture filtering
  };
  VkPhysicalDeviceVulkan11Features enableVk11Features{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
      .shaderDrawParameters = VK_TRUE,  // to support slang
  };
  VkPhysicalDeviceVulkan12Features enabledVk12Features{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
      .pNext = &enableVk11Features,
      .descriptorIndexing = true,
      .shaderSampledImageArrayNonUniformIndexing = true,  // related to descriptor indexing
      .descriptorBindingVariableDescriptorCount = true,   // related to descriptor indexing
      .runtimeDescriptorArray = true,                     // related to descriptor indexing
      .bufferDeviceAddress = true,
  };
  VkPhysicalDeviceVulkan13Features enabledVk13Features{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .pNext = &enabledVk12Features,
      .synchronization2 = true,
      .dynamicRendering = true,
  };
  std::vector<const char*> extensions{1, VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  float priority = 1.f;
  VkDeviceQueueCreateInfo queueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = mGraphicsQueueFamilyIndex,
      .queueCount = 1,
      .pQueuePriorities = &priority,
  };
  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &enabledVk13Features,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queueCreateInfo,
      .enabledExtensionCount = 1,
      .ppEnabledExtensionNames = extensions.data(),
      .pEnabledFeatures = &deviceFeatures,
  };

  checkResult(vkCreateDevice(mPhysicalDevice, &createInfo, 0, &mDevice));

  // get queue
  vkGetDeviceQueue(mDevice, mGraphicsQueueFamilyIndex, 0, &mQueue);
  assert(mQueue);

  // init vma
  VmaVulkanFunctions vkFunctions{
      .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
      .vkCreateImage = vkCreateImage,
  };
  VmaAllocatorCreateInfo allocatorCI{
      .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
      .physicalDevice = mPhysicalDevice,
      .device = mDevice,
      .pVulkanFunctions = &vkFunctions,
      .instance = mContext.mInstance,
  };
  checkResult(vmaCreateAllocator(&allocatorCI, &mAllocator));
  assert(mAllocator);
}

void Vk1Device::destroy() {
  vmaDestroyAllocator(mAllocator);
  vkDestroyDevice(mDevice, 0);
  destroySurface();
}

void Vk1Device::pickPhysicalDevice() {
  auto gpus = mContext.enumeratePhysicalDevices();
  assert(!gpus.empty());
  for (auto gpu : gpus) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(gpu, &props);
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(gpu, &features);

    uint32_t qfCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &qfCount, nullptr);
    std::vector<VkQueueFamilyProperties> qfprops{qfCount};
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &qfCount, qfprops.data());
    for (size_t i = 0; i < qfprops.size(); ++i) {
      if ((qfprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
        mGraphicsQueueFamilyIndex = i;
        mPhysicalDevice = gpu;
        return;
      }
    }
  }
  assert(mPhysicalDevice != VK_NULL_HANDLE);
}

void Vk1Device::createSurface(void* nativeWindowHandle) {
  if (mSurface) {
    return;
  }
#ifdef _WIN32
  VkWin32SurfaceCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
      .hinstance = GetModuleHandle(0),
      .hwnd = (HWND)nativeWindowHandle,
  };
  vkCreateWin32SurfaceKHR(mContext.mInstance, &createInfo, nullptr, &mSurface);
  assert(mSurface);
#endif
}

void Vk1Device::destroySurface() {
  if (mSurface) {
    vkDestroySurfaceKHR(mContext.mInstance, mSurface, 0);
  }
  mSurface = 0;
}

void Vk1Device::createSwapchain(void* nativeWindowHandle, uint32_t width, uint32_t height) {
  createSurface(nativeWindowHandle);
  mSwapchain.create(mSurface, mGraphicsQueueFamilyIndex, width, height);
}
void Vk1Device::destroySwapchain() { mSwapchain.destroy(); }

VkSemaphore Vk1Device::createSemaphore() {
  VkSemaphoreCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };
  VkSemaphore semaphore = 0;
  vkCreateSemaphore(mDevice, &createInfo, nullptr, &semaphore);
  assert(semaphore);
  return semaphore;
}

void Vk1Device::destroySemaphore(VkSemaphore semaphore) {
  if (semaphore) {
    vkDestroySemaphore(mDevice, semaphore, 0);
  }
}

VkFence Vk1Device::createFence() {
  VkFenceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };
  VkFence fence = 0;
  vkCreateFence(mDevice, &createInfo, 0, &fence);
  assert(fence);

  return fence;
}
void Vk1Device::destroyFence(VkFence fence) {
  if (fence) {
    vkDestroyFence(mDevice, fence, 0);
  }
}

VkCommandPool Vk1Device::createCommandPool() {
  VkCommandPoolCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = mGraphicsQueueFamilyIndex,
  };
  VkCommandPool commandPool = 0;
  vkCreateCommandPool(mDevice, &createInfo, 0, &commandPool);
  assert(commandPool);
  return commandPool;
}

void Vk1Device::destroyCommandPool(VkCommandPool pool) {
  if (pool) {
    vkDestroyCommandPool(mDevice, pool, 0);
  }
}

VkRenderPass Vk1Device::createRenderPass() {
  VkAttachmentReference attachment{
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };
  VkAttachmentDescription attachmentDesc{
      .format = mSwapchain.getInfo().format,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };
  VkSubpassDescription subpass{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments = &attachment,
  };
  VkRenderPassCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &attachmentDesc,
      .subpassCount = 1,
      .pSubpasses = &subpass,
  };

  VkRenderPass renderpass = 0;
  vkCreateRenderPass(mDevice, &createInfo, 0, &renderpass);
  assert(renderpass);
  return renderpass;
}

void Vk1Device::destroyRenderPass(VkRenderPass renderpass) {
  if (renderpass) {
    vkDestroyRenderPass(mDevice, renderpass, 0);
  }
}

std::vector<VkFramebuffer> Vk1Device::createFramebuffers(VkRenderPass renderpass) {
  auto swapchainInfo = mSwapchain.getInfo();
  std::vector<VkFramebuffer> framebuffers{swapchainInfo.images.size()};
  for (size_t i = 0; i < framebuffers.size(); ++i) {
    framebuffers[i] = createFramebuffer(renderpass, swapchainInfo.imageViews[i],
                                        swapchainInfo.extent.width, swapchainInfo.extent.height);
  }
  return framebuffers;
}

VkFramebuffer Vk1Device::createFramebuffer(VkRenderPass renderpass, VkImageView imageView,
                                           uint32_t width, uint32_t height) {
  VkFramebufferCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = renderpass,
      .attachmentCount = 1,
      .pAttachments = &imageView,
      .width = width,
      .height = height,
      .layers = 1,
  };
  VkFramebuffer frameBuffer = 0;
  vkCreateFramebuffer(mDevice, &createInfo, 0, &frameBuffer);
  assert(frameBuffer);
  return frameBuffer;
}

void Vk1Device::destroyFramebuffer(VkFramebuffer framebuffer) {
  if (framebuffer) {
    vkDestroyFramebuffer(mDevice, framebuffer, 0);
  }
}

void Vk1Device::destroyPipeline(VkPipeline pipeline) {
  if (pipeline) {
    vkDestroyPipeline(mDevice, pipeline, 0);
  }
}

VkPipelineLayout Vk1Device::createPipelineLayout(VkDescriptorSetLayout setLayout) {
  VkPipelineLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &setLayout,
  };
  VkPipelineLayout layout = 0;
  vkCreatePipelineLayout(mDevice, &createInfo, 0, &layout);
  return layout;
}

void Vk1Device::destroyPipelineLayout(VkPipelineLayout layout) {
  if (layout) {
    vkDestroyPipelineLayout(mDevice, layout, 0);
  }
}

void Vk1Device::waitFences(VkFence fence) {
  vkWaitForFences(mDevice, 1, &fence, VK_TRUE, UINT64_MAX);
}
void Vk1Device::resetFences(VkFence fence) { vkResetFences(mDevice, 1, &fence); }
uint32_t Vk1Device::acquireNextImage(VkSemaphore semaphore) {
  return mSwapchain.acquireNextImage(semaphore);
}
void Vk1Device::present(uint32_t imageIndex, VkSemaphore waitSemaphore) {
  mSwapchain.present(imageIndex, waitSemaphore);
}

Vk1Image Vk1Device::createImage(VkFormat format, uint32_t width, uint32_t height,
                                VkImageUsageFlags usageFlags) {
  VkImageCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = format,
      .extent =
          {
              .width = width,
              .height = height,
              .depth = 1,
          },
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = usageFlags,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
  VmaAllocationCreateInfo allocCI{
      .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
      .usage = VMA_MEMORY_USAGE_AUTO,
  };
  Vk1Image image;
  checkResult(
      vmaCreateImage(mAllocator, &createInfo, &allocCI, &image.image, &image.allocation, nullptr));
  assert(image.image);
  return image;
}

void Vk1Device::destroyImage(Vk1Image& image) {
  if (image.image) {
    vmaDestroyImage(mAllocator, image.image, image.allocation);
  }

  image = {};
}

Vk1ImageView Vk1Device::createImageView(const Vk1Image& vk1Image, VkFormat format,
                                        VkImageSubresourceRange subResourceRange) {
  VkImageViewCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = vk1Image.image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = format,
      .subresourceRange = subResourceRange,
  };
  Vk1ImageView imageView{
      .image = &vk1Image,
  };
  checkResult(vkCreateImageView(mDevice, &createInfo, 0, &imageView.imageView));
  assert(imageView.imageView);
  return imageView;
}

VkImageView Vk1Device::createImageView(VkImage image, VkFormat format,
                                       VkImageSubresourceRange subResourceRange) {
  VkImageViewCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = format,
      .subresourceRange = subResourceRange,
  };
  VkImageView imageView = 0;
  checkResult(vkCreateImageView(mDevice, &createInfo, 0, &imageView));
  assert(imageView);
  return imageView;
}

void Vk1Device::destroyImageView(Vk1ImageView& imageView) {
  if (imageView.imageView) {
    vkDestroyImageView(mDevice, imageView.imageView, 0);
  }

  imageView = {};
}

void Vk1Device::destroyImageView(VkImageView imageView) {
  if (imageView) {
    vkDestroyImageView(mDevice, imageView, 0);
  }
}

Vk1Buffer Vk1Device::createBuffer(uint64_t dataSize, VkBufferUsageFlags usage,
                                  VmaAllocationCreateFlags allocCreateFlag,
                                  VmaMemoryUsage memUsage) {
  VkBufferCreateInfo bufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = dataSize,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };
  VmaAllocationCreateInfo allocInfo{
      .flags = allocCreateFlag,
      .usage = memUsage,
  };
  Vk1Buffer vk1Buffer;
  vmaCreateBuffer(mAllocator, &bufferCreateInfo, &allocInfo, &vk1Buffer.buffer,
                  &vk1Buffer.allocation, &vk1Buffer.allocationInfo);
  assert(vk1Buffer.buffer);

  VkBufferDeviceAddressInfo bufferBdaInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
      .buffer = vk1Buffer.buffer,
  };
  // able to access the buffer in shader
  vk1Buffer.deviceAddress = vkGetBufferDeviceAddress(mDevice, &bufferBdaInfo);
  return vk1Buffer;
}

void Vk1Device::destroyBuffer(Vk1Buffer& buffer) {
  if (buffer.buffer) {
    vmaDestroyBuffer(mAllocator, buffer.buffer, buffer.allocation);
  }
  buffer = {};
}

VkDescriptorSetLayout Vk1Device::createDescriptorSetLayout() {
  std::array<VkDescriptorSetLayoutBinding, 2> bindings;
  bindings[0] = {
      .binding = 0,  // binding index start from
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
  };
  bindings[1] = {
      .binding = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
  };
  VkDescriptorSetLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = bindings.size(),
      .pBindings = bindings.data(),
  };
  VkDescriptorSetLayout setLayout = 0;
  vkCreateDescriptorSetLayout(mDevice, &createInfo, 0, &setLayout);
  assert(setLayout);
  return setLayout;
}

VkDescriptorPool Vk1Device::createDescriptorPool() {
  std::array<VkDescriptorPoolSize, 2> poolSizes;
  poolSizes[0] = {
      .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 2,
  };
  poolSizes[1] = {
      .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 2,
  };
  VkDescriptorPoolCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .maxSets = 2,
      .poolSizeCount = poolSizes.size(),
      .pPoolSizes = poolSizes.data(),
  };
  VkDescriptorPool desPool = 0;
  vkCreateDescriptorPool(mDevice, &createInfo, 0, &desPool);
  assert(desPool);
  return desPool;
}
}  // namespace vk1