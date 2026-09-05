#pragma once

#include "vk1_context.hpp"
#include "vk1_swapchain.hpp"
#include "vk1_buffer.hpp"
#include "vk1_image.hpp"

namespace vk1 {
class Vk1Device {
 public:
  explicit Vk1Device(Vk1Context&& ctx);
  ~Vk1Device();
  Vk1Device(const Vk1Device&) = delete;
  Vk1Device& operator=(const Vk1Device&) = delete;
  Vk1Device(Vk1Device&& rhs) noexcept
      : mContext(std::move(rhs.mContext)),
        mGraphicsQueueFamilyIndex(
            std::exchange(rhs.mGraphicsQueueFamilyIndex, global::invalidIndex)),
        mDevice(std::exchange(rhs.mDevice, VK_NULL_HANDLE)),
        mSurface(std::exchange(rhs.mSurface, VK_NULL_HANDLE)),
        mQueue(std::exchange(rhs.mQueue, VK_NULL_HANDLE)),
        mSwapchain(std::move(rhs.mSwapchain)) {}

  Vk1Device& operator=(Vk1Device&& rhs) noexcept {
    if (this != &rhs) {
      std::swap(mContext, rhs.mContext);
      std::swap(mGraphicsQueueFamilyIndex, rhs.mGraphicsQueueFamilyIndex);
      std::swap(mDevice, rhs.mDevice);
      std::swap(mQueue, rhs.mQueue);
      mSwapchain = std::move(rhs.mSwapchain);
    }
    return *this;
  }

  VkDevice getDevice() { return mDevice; }
  VkQueue getQueue() { return mQueue; }

  void createSwapchain(void* nativeWindowHandle, uint32_t width, uint32_t height);
  void destroySwapchain();

  Vk1Swapchain::Info getSwapchainInfo() const noexcept { return mSwapchain.getInfo(); }

  VkSemaphore createSemaphore();
  void destroySemaphore(VkSemaphore semaphore);

  VkFence createFence();
  void destroyFence(VkFence fence);

  VkCommandPool createCommandPool();
  void destroyCommandPool(VkCommandPool pool);

  Vk1Image createImage(VkFormat format, uint32_t width, uint32_t height,
                       VkImageUsageFlags usageFlags);
  void destroyImage(Vk1Image& image);

  Vk1ImageView createImageView(const Vk1Image& image, VkFormat format,
                               VkImageSubresourceRange subResourceRange);
  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageSubresourceRange subResourceRange);
  void destroyImageView(Vk1ImageView& imageView);
  void destroyImageView(VkImageView imageView);

  Vk1Buffer createBuffer(uint64_t dataSize, VkBufferUsageFlags usage,
                         VmaAllocationCreateFlags allocCreateFlag, VmaMemoryUsage memUsage);
  void destroyBuffer(Vk1Buffer& buffer);

  VkDescriptorSetLayout createDescriptorSetLayout();
  VkDescriptorPool createDescriptorPool();

  VkRenderPass createRenderPass();
  void destroyRenderPass(VkRenderPass renderpass);

  std::vector<VkFramebuffer> createFramebuffers(VkRenderPass renderpass);

  VkFramebuffer createFramebuffer(VkRenderPass renderpass, VkImageView imageView, uint32_t width,
                                  uint32_t height);
  void destroyFramebuffer(VkFramebuffer frameBuffer);

  VkPipelineLayout createPipelineLayout(VkDescriptorSetLayout setLayout);
  void destroyPipelineLayout(VkPipelineLayout layout);

  VkPipeline createPyramidPipeline(VkRenderPass renderpass, VkPipelineLayout layout,
                                   VkPipelineCache pipelineCache, VkShaderModule shaderModule);
  VkPipeline createPyramid2Pipeline(VkRenderPass renderpass, VkPipelineLayout layout,
                                    VkPipelineCache pipelineCache, VkShaderModule shaderModule);
  void destroyPipeline(VkPipeline pipeline);

  void waitFences(VkFence fence);
  void resetFences(VkFence fence);
  uint32_t acquireNextImage(VkSemaphore semaphore);
  void present(uint32_t imageIndex, VkSemaphore waitSemaphore);

  void waitIdle() { vkDeviceWaitIdle(mDevice); }

 private:
  Vk1Context mContext;
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  uint32_t mGraphicsQueueFamilyIndex = global::invalidIndex;
  VkDevice mDevice = VK_NULL_HANDLE;
  VkSurfaceKHR mSurface = VK_NULL_HANDLE;
  VkQueue mQueue = VK_NULL_HANDLE;
  VmaAllocator mAllocator = VK_NULL_HANDLE;
  Vk1Swapchain mSwapchain;

  void init();
  void destroy();
  void pickPhysicalDevice();
  void createSurface(void* nativeWindowHandle);
  void destroySurface();
};
}  // namespace vk1