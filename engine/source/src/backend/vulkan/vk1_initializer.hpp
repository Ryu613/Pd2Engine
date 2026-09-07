#pragma once

#include <span>

#include "vk1_device.hpp"

namespace vk1 {
class Vk1Initializer {
 public:
  class Builder {
   public:
    Builder& enableLayers(std::span<const char*> layerNames) {
      mConfig.instanceLayers.assign(layerNames.begin(), layerNames.end());
      return *this;
    }
    Builder& enableExtensions(std::span<const char*> extNames) {
      mConfig.instanceExtensions.assign(extNames.begin(), extNames.end());
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
    Vk1Device build();

   private:
    Config mConfig;
  };

 private:
  Vk1Initializer() = default;
  ~Vk1Initializer() = default;
};
}  // namespace vk1