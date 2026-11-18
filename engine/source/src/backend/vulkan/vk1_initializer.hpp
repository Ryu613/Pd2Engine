#pragma once

#include <span>

#include "vk1_device.hpp"

namespace vk1 {
class Vk1Initializer {
 public:
  class Builder {
   public:
    Builder& requestInstanceLayers(const std::unordered_map<std::string, RequestMode>& layers) {
      for (const auto& each : layers) {
        mConfig.instanceLayers.emplace(each);
      }
      return *this;
    }
    Builder& requestInstanceExtensions(const std::unordered_map<std::string, RequestMode>& exts) {
      for (const auto& each : exts) {
        mConfig.instanceExtensions.emplace(each);
      }
      return *this;
    }
    Builder& requestDeviceExtensions(const std::unordered_map<std::string, RequestMode>& exts) {
      for (const auto& each : exts) {
        mConfig.deviceExtensions.emplace(each);
      }
      return *this;
    }
    Builder& enableDebug(bool flag = true) {
      mConfig.enableDebug = flag;
      return *this;
    }
    Builder& enableSurface(bool flag = true) {
      mConfig.enableSurface = flag;
      return *this;
    }
    Builder& enableHeadless(bool flag = true) {
      mConfig.enableHeadless = flag;
      return *this;
    }
    Builder& enableBindless(bool flag = true) {
      mConfig.enableBindless = flag;
      return *this;
    }
    Vk1Device build();

   private:
    Config mConfig;
  };

 private:
  Vk1Initializer() = default;
  ~Vk1Initializer() = default;
};
}  // namespace vk1