#include "pd/rendering/renderer.hpp"

namespace pd {
Renderer::Renderer(SceneManager* sceneMgr, Backend* backend)
    : mSceneManager(sceneMgr),
      mBackend(backend),
      mMaterialManager(backend) {}

Renderer::~Renderer() {}

Result<void> Renderer::init() noexcept {
  if (auto res = mMaterialManager.init(); !res) {
    return res;
  }
  return {};
}

Result<void> Renderer::destroy() noexcept {
  if (auto res = mMaterialManager.destroy(); !res) {
    return res;
  }
  return {};
}

void Renderer::renderFrame() noexcept {
  auto ctx = beginFrame();
  doFrame(ctx);
  endFrame(ctx);
}

Renderer::FrameContext Renderer::beginFrame() noexcept {
  auto frameData = mBackend->beginFrame();
  FrameContext ctx{
      .data = frameData,
  };
  ctx.cmdRecorder.setInfo(frameData.frameIndex, frameData.swapchainImageIndex);
  return ctx;
}

void Renderer::doFrame(Renderer::FrameContext& ctx) noexcept {
  auto& recorder = ctx.cmdRecorder;
  recorder.addCmd(BeginRenderingArgs{});
  recorder.addCmd(SetViewportArgs{});
  recorder.addCmd(SetScissorArgs{});
  recorder.addCmd(BindPipelineArgs{});
  recorder.addCmd(EndRenderingArgs{});
}

void Renderer::endFrame(Renderer::FrameContext& ctx) noexcept {
  // 2. end frame
  mBackend->endFrame(ctx.cmdRecorder);
}
}  // namespace pd