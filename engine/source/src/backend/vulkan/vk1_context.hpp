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

  ~Vk1Context();
  Vk1Context();
  Vk1Context(const Vk1Context&) = delete;
  Vk1Context& operator=(const Vk1Context&) = delete;
  Vk1Context(Vk1Context&& rhs) noexcept
      : mInfo(std::exchange(rhs.mInfo, {})),
        mInstance(std::exchange(rhs.mInstance, VK_NULL_HANDLE)),
        mPhysicalDevices(std::exchange(rhs.mPhysicalDevices, {})) {}
  Vk1Context& operator=(Vk1Context&& rhs) noexcept {
    if (this != &rhs) {
      swap(*this, rhs);
    }
    return *this;
  }

  friend void swap(Vk1Context& lhs, Vk1Context& rhs) noexcept {
    using std::swap;
    swap(lhs.mInstance, rhs.mInstance);
    swap(lhs.mPhysicalDevices, rhs.mPhysicalDevices);
    swap(lhs.mInfo, rhs.mInfo);
  }

  VkInstance getInstance() const { return mInstance; }
  const Info& getInfo() const { return mInfo; }

 private:
  friend class Vk1Device;
  Info mInfo;
  VkInstance mInstance = VK_NULL_HANDLE;
  std::vector<VkPhysicalDevice> mPhysicalDevices;

  void init();
  void destroy();
  const std::vector<VkPhysicalDevice>& enumeratePhysicalDevices();
};
}  // namespace vk1