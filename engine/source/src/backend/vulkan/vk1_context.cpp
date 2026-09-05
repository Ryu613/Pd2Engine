#include "vk1_context.hpp"

#include "vk1_helper.hpp"

namespace vk1 {

Vk1Context::Vk1Context() { init(); }
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
  layers.push_back("VK_LAYER_KHRONOS_validation");

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
  };

  checkResult(vkCreateInstance(&createInfo, 0, &mInstance));
  assert(mInstance);
  // fill up info
  mInfo.appInfo = appInfo;
  mInfo.enabledInstanceExtensions = extensions;
  mInfo.enabledInstanceLayers = layers;
}
void Vk1Context::destroy() {
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

}  // namespace vk1