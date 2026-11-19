#pragma once

#include "vk1_context.hpp"
#include "vk1_swapchain.hpp"
#include "vk1_buffer.hpp"
#include "vk1_image.hpp"
#include "vk1_helper.hpp"

namespace vk1 {
class Vk1Device {
 public:
  struct Info {
    std::vector<const char*> enabledExtensions;
  };
  struct QueueInfo {
    uint32_t graphicsQueueFamilyIndex = global::invalidIndex;
    VkQueue graphicsQueue{VK_NULL_HANDLE};
    uint32_t presentQueueFamilyIndex = global::invalidIndex;
    VkQueue presentQueue{VK_NULL_HANDLE};
  };
  explicit Vk1Device(Vk1Context&& ctx);
  ~Vk1Device();
  Vk1Device(const Vk1Device&) = delete;
  Vk1Device& operator=(const Vk1Device&) = delete;
  Vk1Device(Vk1Device&& rhs) noexcept
      : mContext(std::move(rhs.mContext)),
        mPhysicalDevice(std::exchange(rhs.mPhysicalDevice, VK_NULL_HANDLE)),
        mInfo(std::exchange(rhs.mInfo, {})),
        mQueueInfo(std::exchange(rhs.mQueueInfo, {})),
        mDevice(std::exchange(rhs.mDevice, VK_NULL_HANDLE)),
        mSurface(std::exchange(rhs.mSurface, VK_NULL_HANDLE)),
        mSwapchain(std::move(rhs.mSwapchain)) {}

  Vk1Device& operator=(Vk1Device&& rhs) noexcept = delete;

  VkDevice getDevice() { return mDevice; }
  VkPhysicalDevice getPhysicalDevice() { return mPhysicalDevice; }
  VkSurfaceKHR getSurface() { return mSurface; }
  const QueueInfo& getQueueInfo() const { return mQueueInfo; }

  VmaAllocator getAllocator() { return mAllocator; }

  void createSwapchain(void* nativeWindowHandle, uint32_t width, uint32_t height);
  void destroySwapchain();

  const Vk1Swapchain::Info& getSwapchainInfo() const noexcept { return mSwapchain.getInfo(); }

  VkSemaphore createSemaphore();
  void destroySemaphore(VkSemaphore semaphore);

  VkFence createFence();
  void destroyFence(VkFence fence);

  VkCommandPool createCommandPool();
  void destroyCommandPool(VkCommandPool pool);

  Vk1Image createImage(VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usageFlags);
  void destroyImage(Vk1Image& image);

  Vk1ImageView createImageView(const Vk1Image& image, VkFormat format, VkImageSubresourceRange subResourceRange);
  VkImageView createImageView(VkImage image, VkFormat format, VkImageSubresourceRange subResourceRange);
  void destroyImageView(Vk1ImageView& imageView);
  void destroyImageView(VkImageView imageView);

  Vk1Buffer createBuffer(uint64_t dataSize, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocCreateFlag,
                         VmaMemoryUsage memUsage);
  void destroyBuffer(Vk1Buffer& buffer);

  VkDescriptorSetLayout createDescriptorSetLayout();
  VkDescriptorPool createDescriptorPool();

  VkRenderPass createRenderPass();
  void destroyRenderPass(VkRenderPass renderpass);

  std::vector<VkFramebuffer> createFramebuffers(VkRenderPass renderpass);

  VkFramebuffer createFramebuffer(VkRenderPass renderpass, VkImageView imageView, uint32_t width, uint32_t height);
  void destroyFramebuffer(VkFramebuffer frameBuffer);

  VkPipelineLayout createPipelineLayout(VkDescriptorSetLayout setLayout);
  void destroyPipelineLayout(VkPipelineLayout layout);

  VkPipeline createTestPipeline(VkPipelineLayout layout);
  void destroyPipeline(VkPipeline pipeline);

  void waitFences(VkFence fence);
  void resetFences(VkFence fence);
  uint32_t acquireNextImage(VkSemaphore semaphore);
  void present(uint32_t imageIndex, VkSemaphore waitSemaphore);

  void waitIdle() { vkDeviceWaitIdle(mDevice); }

  template <typename T>
  void setDebugName(T handle, VkObjectType objType, const std::string& name) const {
    if (!mContext.getConfig().enableDebug) {
      return;
    }
    const VkDebugUtilsObjectNameInfoEXT nameInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = objType,
        .objectHandle = helper::rawHandleToUint64(handle),
        .pObjectName = name.c_str(),
    };
    auto result = vkSetDebugUtilsObjectNameEXT(mDevice, &nameInfo);
    // checkResult();
  }

 private:
  Vk1Context mContext;
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  Info mInfo;
  QueueInfo mQueueInfo;
  VkDevice mDevice = VK_NULL_HANDLE;
  VkSurfaceKHR mSurface = VK_NULL_HANDLE;
  VmaAllocator mAllocator = VK_NULL_HANDLE;
  Vk1Swapchain mSwapchain;

  void init();
  void destroy();
  void pickPhysicalDevice();
  void initDeviceInfo();
  void createSurface(void* nativeWindowHandle);
  void destroySurface();

  bool enableExt(const std::string& requestExt, const std::vector<VkExtensionProperties>& availableExts);
};
}  // namespace vk1