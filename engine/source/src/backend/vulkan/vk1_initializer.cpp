#include "vk1_initializer.hpp"

#define VOLK_IMPLEMENTATION
#include "volk.h"

namespace vk1 {
Vk1Device Vk1Initializer::Builder::build() {
  volkInitialize();
  Vk1Context ctx;
  volkLoadInstance(ctx.getInstance());
  Vk1Device device(std::move(ctx));
  volkLoadDevice(device.getDevice());
  return device;
}
}  // namespace vk1