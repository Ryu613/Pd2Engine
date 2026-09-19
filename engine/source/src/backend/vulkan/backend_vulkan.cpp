#include "pd/backend/backend.hpp"

#include "vk1_initializer.hpp"
#include "manager/frame_manager.hpp"
#include "manager/resource_registry.hpp"

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
        mResourceRegistry(mVulkanDevice),
        mFrameManager(mVulkanDevice, mResourceRegistry) {}

  ~Impl() {}

  Result<void> init(const BackendConfig& config) noexcept {
    mConfig = config;

    // create swapchain
    mVulkanDevice.createSwapchain(mConfig.windowHandle, mConfig.width, mConfig.height);
    // init resource mgr
    mResourceRegistry.init();
    // init frame data
    mFrameManager.init();

    return {};
  }

  Result<void> destroy() noexcept {
    // destroy frame data
    mFrameManager.destroy();
    // destroy resources
    mResourceRegistry.destroy();
    // destroy swapchain
    mVulkanDevice.destroySwapchain();
    return {};
  }

  pd::FrameData beginFrame() noexcept {
    auto frameData = mFrameManager.beginFrame();
    return {
        .frameIndex = frameData.frameIndex,
        .swapchainImageIndex = frameData.swapchainImageIndex,
    };
  }

  void endFrame(const CommandRecorder& recorder) noexcept {
    mFrameManager.endFrame(recorder);
  }

  PipelineData createGraphicsPipeline(const GraphicsPipelineDesc& desc) noexcept {
    auto layoutHandle = mResourceRegistry.createPipelineLayout({});
    auto pipelineHandle = mResourceRegistry.createGraphicsPipeline(layoutHandle, desc);
    return {layoutHandle, pipelineHandle};
  }

  void destroyGraphicsPipeline(HwGraphicsPipelineHandle handle) noexcept {}

  HwBufferHandle createBuffer(const BufferCreateDesc& bufferCreateDesc) noexcept {
    auto handle = mResourceRegistry.createBuffer(bufferCreateDesc);
    return handle;
  }

  void writeBuffer(const BufferWriteDesc& bufferWriteDesc) noexcept {
    mResourceRegistry.writeBuffer(bufferWriteDesc);
  }

  void destroyBuffer(HwBufferHandle handle) noexcept {
    mResourceRegistry.destroyBuffer(handle);
  }

 private:
  BackendConfig mConfig{};
  vk1::Vk1Device mVulkanDevice;
  vk1::ResourceRegistry mResourceRegistry;
  vk1::FrameManager mFrameManager;
};

Backend::Backend()
    : mImpl(std::make_unique<Impl>()) {}

Backend::~Backend() = default;

Result<void> Backend::init(const BackendConfig& config) noexcept { return mImpl->init(config); }

Result<void> Backend::destroy() noexcept { return mImpl->destroy(); }

pd::FrameData Backend::beginFrame() noexcept { return mImpl->beginFrame(); }

void Backend::endFrame(CommandRecorder recorder) noexcept { mImpl->endFrame(recorder); }

PipelineData Backend::createGraphicsPipeline(const GraphicsPipelineDesc& desc) noexcept {
  return mImpl->createGraphicsPipeline(desc);
}

void Backend::destroyGraphicsPipeline(HwGraphicsPipelineHandle handle) noexcept {
  mImpl->destroyGraphicsPipeline(handle);
}

HwBufferHandle Backend::createBuffer(const BufferCreateDesc& bufferCreateDesc) noexcept {
  return mImpl->createBuffer(bufferCreateDesc);
}
void Backend::writeBuffer(const BufferWriteDesc& bufferWriteDesc) noexcept { mImpl->writeBuffer(bufferWriteDesc); }
void Backend::destroyBuffer(HwBufferHandle handle) noexcept { mImpl->destroyBuffer(handle); }
}  // namespace pd