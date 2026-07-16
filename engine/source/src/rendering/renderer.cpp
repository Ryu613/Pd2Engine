#include "pd/rendering/renderer.hpp"

#include "pd/backend/backend.hpp"
#include "pd/platform/window/window.hpp"

#include "pd/backend/hw_command_recorder.hpp"

#include "pd/rendering/render_pass/skybox_pass.hpp"
#include "pd/rendering/render_pass/forward_pass.hpp"
#include "pd/rendering/render_pass/present_pass.hpp"

#include "pd/scene/scene.hpp"

namespace pd {

Renderer::Renderer(Config config) noexcept
    : mConfig(config) {}

void Renderer::initialize(IBackend* pBackend, IWindow* pWindow, SceneManager* pSceneManager,
                          ResourceManager* pResourceManager) noexcept {
  if (mInitialized) {
    return;
  }
  mBackend = pBackend;
  mWindow = pWindow;
  mSceneManager = pSceneManager;
  mResourceManager = pResourceManager;
  // create swapchain
  const HwSwapchain swapchainOptions{
      .extent =
          {
              .width = mWindow->windowWidth(),
              .height = mWindow->windowHeight(),
          },
  };
  mSwapchain = mBackend->createSwapchain(swapchainOptions);

  // TODO(ryu613): initialize frame context
  mFrames.reserve(static_cast<size_t>(mConfig.inFlightFrame));

  // create render graph based on shading path
  initializeRenderGraph();

  mInitialized = true;
}

Renderer::~Renderer() { destroy(); }

void Renderer::beginFrame() {
  if (mSwapchainDirty) {
    // TODO(ryu613): swapchain resize
  }
  auto result = mBackend->newFrame(mSwapchain);
  if (!result) {
    // TODO(ryu613): deal with swapchain resize & outdate & fail
  }
  //   mRenderGraph.compile();
}

void Renderer::endFrame() {
  // 提交指令
  //   driver.submitFrame();
  // 呈现
  if (auto res = mBackend->presentFrame(mSwapchain); !res) {
    // TODO(ryu613) : deal with swapchain resize or other error
  }
  // 结束当前帧
  if (auto res = mBackend->endFrame(mSwapchain); !res) {
    // TODO(ryu613): deal with error
  }
  // TODO(ryu613): clean per-frame resources
}

void Renderer::renderFrame() {
  // shorcut: for testing only
  auto& entityManager = mSceneManager->getEntityManager();
  auto& scene = mSceneManager->getScene();
  auto& renderables = mSceneManager->getRenderables();
  for (auto& renderable : renderables) {
    render(renderable);
  }
  // 指令重置及开始录制
  //   mBackend->startCmdRecording();
  //   mRenderGraph.execute();
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
      // mRenderGraph.addPass<ForwardPass>();
      //   mRenderGraph.addPass<SkyBoxPass>();
      break;
    case Deferred:
      PD_ASSERT_MSG(false, "deferred shading not implemented");
    case PathTracing:
      PD_ASSERT_MSG(false, "path tracing not implemented");
    default:
      PD_ASSERT_MSG(false, "initialize render graph failed!");
  }
  // mRenderGraph.addPass<PresentPass>();
  // mRenderGraph.setup();
}

void Renderer::render(Renderable& renderable) noexcept {
  // if window is not minimized
  auto cmdRecorder = mBackend->getCommandRecorder();
  for (auto& primitive : renderable.pMeshResource->primitives()) {
    mBackend->beginCmdRecord(cmdRecorder);
    GraphicsState state;
    mBackend->setGraphicsState(cmdRecorder, state);
    DrawIndexedCommand drawCmd;
    mBackend->drawIndexed(cmdRecorder, drawCmd);
  }
  mBackend->endCmdRecord(cmdRecorder);
  mBackend->submitFrame(mSwapchain, cmdRecorder);
}
}  // namespace pd