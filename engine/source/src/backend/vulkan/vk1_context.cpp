#include "vk1_context.hpp"

#include "vk1_helper.hpp"

namespace vk1 {
namespace {
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) {
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
  initLayersAndExts();
  // only support 1.3
  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = mConfig.appName.c_str(),
      .pEngineName = "vk1",
      .apiVersion = VK_API_VERSION_1_3,
  };

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(mInfo.enabledInstanceLayers.size()),
      .ppEnabledLayerNames = mInfo.enabledInstanceLayers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(mInfo.enabledInstanceExtensions.size()),
      .ppEnabledExtensionNames = mInfo.enabledInstanceExtensions.data(),
  };

  if (mConfig.enableDebug) {
    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
    };
    createInfo.pNext = &messengerCreateInfo;
  }

  checkResult(vkCreateInstance(&createInfo, 0, &mInstance));
  assert(mInstance);
  // fill up info
  mInfo.appInfo = appInfo;
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
      .messageSeverity =
          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback,
  };

  checkResult(vkCreateDebugUtilsMessengerEXT(mInstance, &messengerCreateInfo, 0, &mDebugMsgr));
  assert(mDebugMsgr);
}

void Vk1Context::initLayersAndExts() {
  // basic
  mConfig.instanceExtensions[VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME] = RequestMode::Required;
  // by configurations
  if (mConfig.enableSurface) {
    mConfig.instanceExtensions[VK_KHR_SURFACE_EXTENSION_NAME] = RequestMode::Required;
#if defined(_WIN32)
    mConfig.instanceExtensions[VK_KHR_WIN32_SURFACE_EXTENSION_NAME] = RequestMode::Required;
#else
#error "platform not supported"
#endif
    mConfig.deviceExtensions[VK_KHR_SWAPCHAIN_EXTENSION_NAME] = RequestMode::Required;
  }
  if (mConfig.shaderLang == ShaderLang::Slang) {
    mConfig.deviceExtensions[VK_KHR_SPIRV_1_4_EXTENSION_NAME] = RequestMode::Required;
    mConfig.deviceExtensions[VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME] = RequestMode::Required;
    mConfig.deviceExtensions[VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME] = RequestMode::Required;
  }
  if (mConfig.enableDebug) {
    mConfig.instanceLayers["VK_LAYER_KHRONOS_validation"] = RequestMode::Required;
    mConfig.instanceExtensions[VK_EXT_DEBUG_UTILS_EXTENSION_NAME] = RequestMode::Required;
  }

  // check requests
  uint32_t requestedApiVersion = VK_API_VERSION_1_3;
  uint32_t instanceApiVersion = 0;
  checkResult(vkEnumerateInstanceVersion(&instanceApiVersion));
  assert(instanceApiVersion >= requestedApiVersion);

  uint32_t instanceLayerCount = 0;
  checkResult(vkEnumerateInstanceLayerProperties(&instanceLayerCount, 0));
  assert(instanceLayerCount > 0);
  std::vector<VkLayerProperties> instanceLayers{instanceLayerCount};
  checkResult(vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers.data()));

  for (const auto& eachLayer : mConfig.instanceLayers) {
    if (!enableLayer(eachLayer.first, instanceLayers)) {
      assert(eachLayer.second == RequestMode::Optional);
    }
  }

  uint32_t instanceExtsCount = 0;
  checkResult(vkEnumerateInstanceExtensionProperties(0, &instanceExtsCount, 0));
  assert(instanceExtsCount > 0);
  std::vector<VkExtensionProperties> instanceExts{instanceExtsCount};
  checkResult(vkEnumerateInstanceExtensionProperties(0, &instanceExtsCount, instanceExts.data()));

  auto validationLayerContains = std::ranges::any_of(mInfo.enabledInstanceLayers, [](const char* rangeValue) {
    return strcmp(rangeValue, "VK_LAYER_KHRONOS_validation") == 0;
  });
  if (validationLayerContains) {
    const std::string validationLayerName = "VK_LAYER_KHRONOS_validation";
    uint32_t validationLayerInstanceExtsCount;
    checkResult(
        vkEnumerateInstanceExtensionProperties(validationLayerName.c_str(), &validationLayerInstanceExtsCount, 0));
    std::vector<VkExtensionProperties> validationExts{validationLayerInstanceExtsCount};
    checkResult(vkEnumerateInstanceExtensionProperties(validationLayerName.c_str(), &validationLayerInstanceExtsCount,
                                                       validationExts.data()));
    instanceExts.insert(instanceExts.end(), validationExts.begin(), validationExts.end());
  }

  for (const auto& ext : mConfig.instanceExtensions) {
    if (!enableExt(ext.first, instanceExts)) {
      assert(ext.second == RequestMode::Optional);
    }
  }
}

bool Vk1Context::enableLayer(const std::string& requestLayer, const std::vector<VkLayerProperties>& availableLayers) {
  bool isAvailable = std::ranges::any_of(availableLayers, [&requestLayer](const auto& availableLayer) {
    return requestLayer == availableLayer.layerName;
  });
  if (isAvailable) {
    bool isEnabled = std::ranges::any_of(mInfo.enabledInstanceLayers, [&requestLayer](const auto& enabledLayer) {
      return requestLayer == enabledLayer;
    });
    if (!isEnabled) {
      mInfo.enabledInstanceLayers.emplace_back(requestLayer.c_str());
    }
  }
  return isAvailable;
}

bool Vk1Context::enableExt(const std::string& requestExt, const std::vector<VkExtensionProperties>& availableExts) {
  bool isAvailable = std::ranges::any_of(
      availableExts, [&requestExt](const auto& availableExt) { return requestExt == availableExt.extensionName; });
  if (isAvailable) {
    bool isEnabled = std::ranges::any_of(mInfo.enabledInstanceExtensions,
                                         [&requestExt](auto const& enabledExt) { return requestExt == enabledExt; });
    if (!isEnabled) {
      mInfo.enabledInstanceExtensions.emplace_back(requestExt.c_str());
    }
  }
  return isAvailable;
}
}  // namespace vk1