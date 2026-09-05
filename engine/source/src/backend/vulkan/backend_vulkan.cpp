#include "pd/backend/backend.hpp"

#include "vk1_initializer.hpp"

namespace pd {
namespace {
vk1::Vk1Device createVulkanDevice() {
  vk1::Vk1Initializer::Builder builder;
  return builder  //
      .enableDebug()
      .enableSurface()
      .build();
}
}  // namespace
class Backend::Impl {
 public:
  Impl()
      : mVulkanDevice(createVulkanDevice()) {}

  ~Impl() {}

  Result<void> init(const BackendConfig& config) noexcept {
    mConfig = config;

    mVulkanDevice.createSwapchain(mConfig.windowHandle, mConfig.width, mConfig.height);

    return {};
  }

  Result<void> destroy() noexcept { return {}; }

 private:
  BackendConfig mConfig{};
  vk1::Vk1Device mVulkanDevice;
};

Backend::Backend()
    : mImpl(std::make_unique<Impl>()) {}

Backend::~Backend() = default;

Result<void> Backend::init(const BackendConfig& config) noexcept { return mImpl->init(config); }

Result<void> Backend::destroy() noexcept { return mImpl->destroy(); }
}  // namespace pd