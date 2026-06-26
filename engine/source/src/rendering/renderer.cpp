#include "pd/rendering/renderer.hpp"

#include "pd/backend/backend.hpp"
#include "pd/platform/window/window.hpp"

#include "pd/rendering/render_pass/skybox_pass.hpp"
#include "pd/rendering/render_pass/forward_pass.hpp"
#include "pd/rendering/render_pass/present_pass.hpp"

namespace pd {

Renderer::Renderer(Config config) noexcept
    : mConfig(config) {}

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

  // create render graph based on shading path
  initializeRenderGraph();

  mInitialized = true;
}

Renderer::~Renderer() { destroy(); }

void Renderer::beginFrame() {
  if (mSwapchainDirty) {
    // TODO(author): swapchain resize
  }
  auto result = mBackend->acquireNextFrame(mSwapchain);
  // TODO(author): deal with swapchain resize & outdate & fail
}

void Renderer::endFrame() {
  // 提交指令
  //   driver.submitFrame();
  // 呈现
  //   driver.presentFrame();
  // 结束当前帧
  //   driver.endFrame();
}

void Renderer::renderFrame() {
  // 指令重置及开始录制
  //   mBackend->startCmdRecording();
  /*
   * TODO: render graph
   */
  mRenderGraph.execute();
  // 停止指令录制
  //   mBackend->endCmdRecording();
}

void Renderer::destroy() noexcept {
  if (!mInitialized) {
    return;
  }
  // destroy swapchain
  mBackend->destroySwapchain(mSwapchain);

  mInitialized = false;
}

void Renderer::initializeRenderGraph() noexcept {
  mRenderGraph.reset();
  switch (mConfig.renderPipeline) {
    using enum RenderPipeline;
    case Forward:
      //   mRenderGraph.addPass<GBufferPass>();
      //   mRenderGraph.addPass<ForwardPass>();
      //   mRenderGraph.addPass<SkyBoxPass>();
      break;
    case Deferred:
      PD_ASSERT_MSG(false, "deferred shading not implemented");
    case PathTracing:
      PD_ASSERT_MSG(false, "path tracing not implemented");
    default:
      PD_ASSERT_MSG(false, "initialize render graph failed! RenderPipeline error!");
  }
  mRenderGraph.addPass<PresentPass>();
  mRenderGraph.compile();
}
}  // namespace pd