#include "pd/backend/vulkan/vulkan_context.hpp"

#define VOLK_IMPLEMENTATION
#include "volk.h"
#include "VkBootstrap.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace pd {
VulkanContext::VulkanContext(Config config) noexcept
    : mConfig(std::move(config)) {
  init();
}

VulkanContext::~VulkanContext() { destroy(); }

void VulkanContext::init() noexcept {
  volkInitialize();
  vkb::InstanceBuilder instanceBuilder;
  auto builder = instanceBuilder.set_app_name(mConfig.appName.c_str())
                     .use_default_debug_messenger()
                     .require_api_version(1, 3, 0)
                     .enable_extensions(mConfig.requiredInstanceExtensions);
  if (mConfig.enableValidationLayer) {
    builder.request_validation_layers();
  }
  auto initRet = builder.build();
  if (!initRet) {
    log::error(initRet.error().message().data());
    PD_ASSERT_MSG(false, "vulkan instance create failed!");
  }
  vkb::Instance instance = initRet.value();
  mInstance = instance.instance;
  mDebugMsgr = instance.debug_messenger;

  volkLoadInstance(mInstance);

  if (!createSurface(mConfig.nativeWindowHandle)) {
    PD_ASSERT_MSG(false, "vulkan surface creation failed!");
  }

  // ref:
  // 1.
  // https://docs.vulkan.org/samples/latest/samples/extensions/descriptor_indexing/README.html
  // 2.
  // https://docs.vulkan.org/refpages/latest/refpages/source/VkPhysicalDeviceDescriptorIndexingFeatures.html
  // 3.
  // https://docs.vulkan.org/refpages/latest/refpages/source/VkPhysicalDeviceFeatures.html#_members
  VkPhysicalDeviceVulkan13Features features13{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .synchronization2 = VK_TRUE,
      .dynamicRendering = VK_TRUE,
  };

  VkPhysicalDeviceVulkan12Features features12{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
      .descriptorIndexing = VK_TRUE,
      .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
      .descriptorBindingVariableDescriptorCount = VK_TRUE,
      .runtimeDescriptorArray = VK_TRUE,
      .bufferDeviceAddress = VK_TRUE,
  };
  VkPhysicalDeviceFeatures features10{
      .samplerAnisotropy = VK_TRUE,
  };
  vkb::PhysicalDeviceSelector physicalDeviceSelector(instance);
  auto phyDeviceRet = physicalDeviceSelector.set_minimum_version(1, 3)
                          .set_required_features(features10)
                          .set_required_features_12(features12)
                          .set_required_features_13(features13)
                          .set_surface(mSurface)
                          .select();
  if (!phyDeviceRet) {
    log::error(phyDeviceRet.error().message().data());
    if (phyDeviceRet.error() == vkb::PhysicalDeviceError::no_suitable_device) {
      const auto& detailed_reasons = phyDeviceRet.detailed_failure_reasons();
      if (!detailed_reasons.empty()) {
        log::error("GPU Selection failure reasons:\n");
        for (const std::string& reason : detailed_reasons) {
          log::error(reason + "\n");
        }
      }
    }
  }

  const auto& phyDevice = phyDeviceRet.value();
  mPhysicalDevice = phyDevice.physical_device;

  vkb::DeviceBuilder deviceBuilder(phyDevice);

  auto deviceRet = deviceBuilder.build();
  if (!deviceRet) {
    log::error(deviceRet.error().message().data());
    PD_ASSERT_MSG(false, "vulkan device create failed!");
  }
  const auto& device = deviceRet.value();
  mDevice = device.device;

  volkLoadDevice(mDevice);

  // get queue info
  auto graphicsQueueRet = device.get_queue(vkb::QueueType::graphics);
  if (!graphicsQueueRet) {
    log::error(graphicsQueueRet.error().message().data());
    PD_ASSERT_MSG(false, "vulkan graphics queue info fetch failed!");
  }
  mQueueInfo.graphicsQueue = graphicsQueueRet.value();
  mQueueInfo.graphicsQueueFamilyIndex =
      device.get_queue_index(vkb::QueueType::graphics).value();

  auto presentQueueRet = device.get_queue(vkb::QueueType::present);
  if (!presentQueueRet) {
    log::error(graphicsQueueRet.error().message().data());
    PD_ASSERT_MSG(false, "vulkan present queue info fetch failed!");
  }
  mQueueInfo.presentQueue = presentQueueRet.value();
  mQueueInfo.presentQueueFamilyIndex =
      device.get_queue_index(vkb::QueueType::present).value();

  // init vma
  const VmaVulkanFunctions vmaFuncs{
      .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
      .vkCreateImage = vkCreateImage,
  };
  const VmaAllocatorCreateInfo allocatorInfo{
      .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
      .physicalDevice = mPhysicalDevice,
      .device = mDevice,
      .pVulkanFunctions = &vmaFuncs,
      .instance = mInstance,
  };

  VK_CHECK(vmaCreateAllocator(&allocatorInfo, &mVmaAllocator));
}

void VulkanContext::destroy() noexcept {
  vkDeviceWaitIdle(mDevice);
  vmaDestroyAllocator(mVmaAllocator);
  vkDestroyDevice(mDevice, nullptr);
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMsgr, nullptr);
  vkDestroyInstance(mInstance, nullptr);

  volkFinalize();
}

bool VulkanContext::createSurface(void* nativeWindowHandle) noexcept {
#ifdef _WIN32
  VkWin32SurfaceCreateInfoKHR surfaceInfo{
      .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
      .hinstance = GetModuleHandle(nullptr),
      .hwnd = (HWND)nativeWindowHandle,
  };
  VK_CHECK(vkCreateWin32SurfaceKHR(mInstance, &surfaceInfo, nullptr, &mSurface));
#else
  PD_ASSERT_MSG(false, "illegal operation!");
#endif
  return true;
}
}  // namespace pd