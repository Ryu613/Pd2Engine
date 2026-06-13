#include "pd/core/layers/engine_layer.hpp"

#ifdef BACKEND_VULKAN
#include "pd/platform/window/sdl3/sdl3_helper.hpp"
#include "pd/backend/vulkan/backend_vulkan.hpp"
#endif

namespace pd {
using Error = Layer::Error;
EngineLayer::EngineLayer(Window* window) noexcept
    : mWindow(window) {}

EngineLayer::LayerResult<void> EngineLayer::onAttached() noexcept {
  if (!mWindow->create()) {
    log::error("window creation failed!");
    return std::unexpected<Error>(Error::InitializeFailed);
  }
// 初始化渲染后端
#ifdef BACKEND_VULKAN
  // TODO(author): 暂时不区分SDL
  VulkanContext::Config vulkanConfig{
      .appName = mWindow->windowTitle(),
      .nativeWindowHandle =
          SDL3Helper::getNativeWindowHandle(static_cast<SDL_Window*>(mWindow->handle())),
      .requiredInstanceExtensions = SDL3Helper::getVulkanInstanceExtensions(),
  };
  VulkanContext context{vulkanConfig};
  mBackend = std::make_unique<BackendVulkan>(std::move(context));
#else
  PD_ASSERT_MSG(false, "BACKEND PARAM ERROR!");
#endif
  return {};
}
EngineLayer::LayerResult<void> EngineLayer::onDetached() noexcept {
  mBackend.reset();
  mWindow->close();
}
void EngineLayer::onUpdate() noexcept {}
}  // namespace pd