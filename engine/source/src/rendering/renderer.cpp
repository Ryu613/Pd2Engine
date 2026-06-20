#include "pd/rendering/renderer.hpp"

#include "pd/backend/backend.hpp"
#include "pd/platform/window/window.hpp"
// #include "pd/platform/rhi/rhi_api.hpp"

namespace pd {

Renderer::Renderer(IBackend* pBackend, IWindow* pWindow) noexcept
    : mBackend(pBackend),
      mWindow(pWindow) {
  init();
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

void Renderer::init() noexcept {
  // create swapchain
  const HwSwapchain swapchainOptions{
      .extent =
          {
              .width = mWindow->windowWidth(),
              .height = mWindow->windowHeight(),
          },
  };
  mSwapchain = mBackend->createSwapchain(swapchainOptions);
}

void Renderer::destroy() noexcept {
  // destroy swapchain
  mBackend->destroySwapchain(mSwapchain);
}
}  // namespace pd