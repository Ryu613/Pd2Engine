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

  void updateFrameConstants(const FrameConstantsDesc& frameConstantsDesc) noexcept {
    mFrameManager.updateFrame(frameConstantsDesc);
  }

  void endFrame(const CommandRecorder& recorder) noexcept { mFrameManager.endFrame(recorder); }

  void waitIdle() noexcept { mVulkanDevice.waitIdle(); }

  PipelineData createGraphicsPipeline(const GraphicsPipelineCreateDesc& desc) noexcept {
    auto layoutHandle = mResourceRegistry.createPipelineLayout({});
    auto newDesc = desc;
    newDesc.layout = layoutHandle;
    auto pipelineHandle = mResourceRegistry.createGraphicsPipeline(newDesc);
    return {layoutHandle, pipelineHandle};
  }

  void destroyGraphicsPipeline(const GraphicsPipelineDestroyDesc& desc) noexcept {
    mResourceRegistry.destroyGraphicsPipeline(desc);
  }

  HwBufferHandle createBuffer(const BufferCreateDesc& bufferCreateDesc) noexcept {
    auto handle = mResourceRegistry.createBuffer(bufferCreateDesc);
    return handle;
  }

  void writeBuffer(const BufferWriteDesc& bufferWriteDesc) noexcept { mResourceRegistry.writeBuffer(bufferWriteDesc); }

  void destroyBuffer(HwBufferHandle handle) noexcept { mResourceRegistry.destroyBuffer(handle); }

  HwShaderModuleHandle createShaderModule(const ShaderModuleCreateDesc& shaderModuleCreateDesc) noexcept {
    auto handle = mResourceRegistry.createShaderModule(shaderModuleCreateDesc);
    return handle;
  }
  void destroyShaderModule(HwShaderModuleHandle handle) noexcept { mResourceRegistry.destroyShaderModule(handle); }

  HwTextureHandle createTexture(const TextureCreateDesc& textureCreateDesc) noexcept {
    auto handle = mResourceRegistry.createTexture(textureCreateDesc);
    return handle;
  }

  void destroyTexture(HwTextureHandle handle) noexcept { mResourceRegistry.destroyTexture(handle); }

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

void Backend::updateFrameConstants(const FrameConstantsDesc& frameConstantsDesc) noexcept {
  mImpl->updateFrameConstants(frameConstantsDesc);
}

void Backend::endFrame(CommandRecorder recorder) noexcept { mImpl->endFrame(recorder); }

void Backend::waitIdle() noexcept { mImpl->waitIdle(); }

PipelineData Backend::createGraphicsPipeline(const GraphicsPipelineCreateDesc& desc) noexcept {
  return mImpl->createGraphicsPipeline(desc);
}

void Backend::destroyGraphicsPipeline(const GraphicsPipelineDestroyDesc& desc) noexcept {
  mImpl->destroyGraphicsPipeline(desc);
}

HwBufferHandle Backend::createBuffer(const BufferCreateDesc& bufferCreateDesc) noexcept {
  return mImpl->createBuffer(bufferCreateDesc);
}
void Backend::writeBuffer(const BufferWriteDesc& bufferWriteDesc) noexcept { mImpl->writeBuffer(bufferWriteDesc); }
void Backend::destroyBuffer(HwBufferHandle handle) noexcept { mImpl->destroyBuffer(handle); }

HwShaderModuleHandle Backend::createShaderModule(const ShaderModuleCreateDesc& shaderModuleCreateDesc) noexcept {
  return mImpl->createShaderModule(shaderModuleCreateDesc);
}
void Backend::destroyShaderModule(HwShaderModuleHandle handle) noexcept { mImpl->destroyShaderModule(handle); }

HwTextureHandle Backend::createTexture(const TextureCreateDesc& textureCreateDesc) noexcept {
  return mImpl->createTexture(textureCreateDesc);
}
void Backend::destroyTexture(HwTextureHandle handle) noexcept { mImpl->destroyTexture(handle); }
}  // namespace pd