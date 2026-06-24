#include "pd/rendering/renderer.hpp"

#include "pd/backend/backend.hpp"
#include "pd/platform/window/window.hpp"
// #include "pd/platform/rhi/rhi_api.hpp"

namespace pd {

void Renderer::initialize(IBackend* pBackend, IWindow* pWindow) noexcept {
  if (mInitialized) {
    return;
  }
  mBackend = pBackend;
  mWindow = pWindow;
  // create swapchain
  const HwSwapchain swapchainOptions{
      .extent =
          {
              .width = mWindow->windowWidth(),
              .height = mWindow->windowHeight(),
          },
  };
  mSwapchain = mBackend->createSwapchain(swapchainOptions);
  mInitialized = true;
}

Renderer::~Renderer() { destroy(); }

void Renderer::beginFrame() {
  if (mSwapchainDirty) {
    // TODO(author): swapchain resize
  }
  //   mBackend->acquireNextFrame();
  // 指令重置及开始录制
  //   mBackend->startCmdRecording();
}

void Renderer::endFrame() {
  //   auto& driver = static_cast<VulkanDriver&>(mDriver);
  // 停止指令录制
  //   driver.endCmdRecording();
  // 提交指令
  //   driver.submitFrame();
  // 呈现
  //   driver.presentFrame();
  // 结束当前帧
  //   driver.endFrame();
}

void Renderer::renderFrame(View& view) {
  // prepare in-frame view & scene data(using job system)
  // view.prepare();
  // build render graph
  // compile render graph(build dependency graph)
  // execute render graph(record all rendering commands)
}

void Renderer::destroy() noexcept {
  if (!mInitialized) {
    return;
  }
  // destroy swapchain
  mBackend->destroySwapchain(mSwapchain);

  mInitialized = false;
}
}  // namespace pd