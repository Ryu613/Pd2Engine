#include "vk1_context.hpp"

#include "vk1_helper.hpp"

namespace vk1 {
namespace {
VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      LOG_DEBUG("validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      LOG_INFO("validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      LOG_WARN("validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      LOG_ERROR("validation layer: {}", pCallbackData->pMessage);
      break;
    default:
      LOG_TRACE("validation layer: {}", pCallbackData->pMessage);
      break;
  }

  return VK_FALSE;
}
}  // namespace

Vk1Context::Vk1Context(const Config& config)
    : mConfig(config) {
  init();
}
Vk1Context::~Vk1Context() { destroy(); }

void Vk1Context::init() {
  // create instance
  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .apiVersion = VK_API_VERSION_1_3,
  };

  std::vector<const char*> extensions;
  extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
  std::vector<const char*> layers;
  if (mConfig.enableDebug) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    layers.push_back("VK_LAYER_KHRONOS_validation");
  }

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
  };

  if (mConfig.enableDebug) {
    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
    };
    createInfo.pNext = &messengerCreateInfo;
  }

  checkResult(vkCreateInstance(&createInfo, 0, &mInstance));
  assert(mInstance);
  // fill up info
  mInfo.appInfo = appInfo;
  mInfo.enabledInstanceExtensions = extensions;
  mInfo.enabledInstanceLayers = layers;
}
void Vk1Context::destroy() {
  if (mDebugMsgr) {
    vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMsgr, 0);
  }
  if (mInstance) {
    vkDestroyInstance(mInstance, 0);
    mInstance = VK_NULL_HANDLE;
  }
}

const std::vector<VkPhysicalDevice>& Vk1Context::enumeratePhysicalDevices() {
  if (mPhysicalDevices.empty()) {
    // get gpus
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(mInstance, &count, nullptr);
    assert(count != 0);
    mPhysicalDevices.resize(count);
    vkEnumeratePhysicalDevices(mInstance, &count, mPhysicalDevices.data());
  }

  return mPhysicalDevices;
}

void Vk1Context::setDebugUtilsMessenger() {
  const VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback,
  };

  checkResult(vkCreateDebugUtilsMessengerEXT(mInstance, &messengerCreateInfo, 0, &mDebugMsgr));
  assert(mDebugMsgr);
}

}  // namespace vk1