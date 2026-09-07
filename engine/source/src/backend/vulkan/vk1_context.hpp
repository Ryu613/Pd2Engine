#pragma once

#include "vk1_common.hpp"

namespace vk1 {
struct Vk1PhysicalDevice {
  VkPhysicalDevice physicalDevice = 0;
  VkPhysicalDeviceProperties props{};
  VkPhysicalDeviceFeatures features{};
  uint32_t graphicsQueueFamilyIndex = 0;
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

  Vk1Context& operator=(Vk1Context&& rhs) noexcept {
    if (this != &rhs) {
      swap(*this, rhs);
    }
    return *this;
  }

  friend void swap(Vk1Context& lhs, Vk1Context& rhs) noexcept {
    using std::swap;
    swap(lhs.mPhysicalDevices, rhs.mPhysicalDevices);
    swap(lhs.mInstance, rhs.mInstance);
    swap(lhs.mDebugMsgr, rhs.mDebugMsgr);
    swap(lhs.mInfo, rhs.mInfo);
    swap(lhs.mConfig, rhs.mConfig);
  }

  VkInstance getInstance() const { return mInstance; }

  const Info& getInfo() const { return mInfo; }

  const Config& getConfig() const { return mConfig; }

  void setDebugUtilsMessenger();

  const std::vector<VkPhysicalDevice>& enumeratePhysicalDevices();

 private:
  Config mConfig;
  Info mInfo;
  VkInstance mInstance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT mDebugMsgr{VK_NULL_HANDLE};
  std::vector<VkPhysicalDevice> mPhysicalDevices;

  void init();
  void destroy();
};
}  // namespace vk1