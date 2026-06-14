#include "pd/rendering/layer/engine_layer.hpp"

#ifdef BACKEND_VULKAN
#include "pd/backend/vulkan/backend_vulkan.hpp"
#endif

namespace pd {
using Error = Layer::Error;
EngineLayer::EngineLayer(Window* window) noexcept
    : mWindow(window) {}

EngineLayer::LayerResult<void> EngineLayer::onAttached() noexcept {
  log::info("engine layer initializing...");
  if (!mWindow->create()) {
    log::error("window creation failed!");
    return std::unexpected<Error>(Error::InitializeFailed);
  }
// 初始化渲染后端
#ifdef BACKEND_VULKAN
  VulkanContext::Config vulkanConfig{
      .appName = mWindow->windowTitle(),
      .nativeWindowHandle = mWindow->nativeHandle(),
      .requiredInstanceExtensions = mWindow->getVulkanInstanceExtensions()};
  VulkanContext context{vulkanConfig};
  mBackend = std::make_unique<BackendVulkan>(std::move(context));
#else
  PD_ASSERT_MSG(false, "BACKEND PARAM ERROR!");
#endif
  return {};
}
EngineLayer::LayerResult<void> EngineLayer::onDetached() noexcept {
  log::info("engine layer shutting down");
  mBackend.reset();
  mWindow->close();
}
void EngineLayer::onUpdate() noexcept { render(); }
}  // namespace pd