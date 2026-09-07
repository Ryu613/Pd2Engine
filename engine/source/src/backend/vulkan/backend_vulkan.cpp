#include "pd/backend/backend.hpp"

#include "vk1_initializer.hpp"
#include "manager/frame_manager.hpp"

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
      : mVulkanDevice(createVulkanDevice()),
        mFrameManager(mVulkanDevice) {}

  ~Impl() {}

  Result<void> init(const BackendConfig& config) noexcept {
    mConfig = config;

    // create swapchain
    mVulkanDevice.createSwapchain(mConfig.windowHandle, mConfig.width, mConfig.height);
    // init frame data
    mFrameManager.init();

    return {};
  }

  Result<void> destroy() noexcept {
    // destroy frame data
    mFrameManager.destroy();
    // destroy swapchain
    mVulkanDevice.destroySwapchain();
    return {};
  }

 private:
  BackendConfig mConfig{};
  vk1::Vk1Device mVulkanDevice;
  vk1::FrameManager<vk1::global::maxInflightFrames> mFrameManager;
};

Backend::Backend()
    : mImpl(std::make_unique<Impl>()) {}

Backend::~Backend() = default;

Result<void> Backend::init(const BackendConfig& config) noexcept { return mImpl->init(config); }

Result<void> Backend::destroy() noexcept { return mImpl->destroy(); }
}  // namespace pd