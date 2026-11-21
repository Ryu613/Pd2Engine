#pragma once

#include "vk1_common.hpp"

namespace vk1 {
struct Vk1PhysicalDevice {
  VkPhysicalDevice physicalDevice = 0;
  VkPhysicalDeviceProperties props{};
  VkPhysicalDeviceFeatures features{};
};

class Vk1Context {
 public:
  struct Info {
    VkApplicationInfo appInfo{};
    std::vector<const char*> enabledInstanceLayers;
    std::vector<const char*> enabledInstanceExtensions;
  };

  Vk1Context(const Config& config);
  ~Vk1Context();
  Vk1Context(const Vk1Context&) = delete;
  Vk1Context& operator=(const Vk1Context&) = delete;
  Vk1Context(Vk1Context&& rhs) noexcept
      : mConfig(std::exchange(rhs.mConfig, {})),
        mInfo(std::exchange(rhs.mInfo, {})),
        mInstance(std::exchange(rhs.mInstance, VK_NULL_HANDLE)),
        mDebugMsgr(std::exchange(rhs.mDebugMsgr, VK_NULL_HANDLE)),
        mPhysicalDevices(std::exchange(rhs.mPhysicalDevices, {})) {}

  Vk1Context& operator=(Vk1Context&& rhs) noexcept = delete;

  VkInstance getInstance() const { return mInstance; }

  const Info& getInfo() const { return mInfo; }

  const Config& getConfig() const { return mConfig; }

  void setDebugUtilsMessenger();

  const std::vector<Vk1PhysicalDevice>& enumeratePhysicalDevices();

 private:
  Config mConfig;
  Info mInfo;
  VkInstance mInstance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT mDebugMsgr{VK_NULL_HANDLE};
  std::vector<Vk1PhysicalDevice> mPhysicalDevices;

  void init();
  void destroy();

  void initLayersAndExts();

  bool enableLayer(const std::string& requestLayer, const std::vector<VkLayerProperties>& availableLayers);
  bool enableExt(const std::string& requestExt, const std::vector<VkExtensionProperties>& availableExts);
};
}  // namespace vk1