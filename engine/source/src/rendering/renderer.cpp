#include "pd/rendering/renderer.hpp"

namespace pd {
Renderer::Renderer(SceneManager* sceneMgr, Backend* backend)
    : mSceneManager(sceneMgr),
      mBackend(backend) {}

Renderer::~Renderer() {}

Result<void> Renderer::init() noexcept {
  // 1. init backend frames
  // 3. init render graph
  return {};
}

Result<void> Renderer::destroy() noexcept {
  // 2. destroy render graph
  // 3. destroy in-frame data
  // 4. destroy frame context
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
  return ctx;
}

void Renderer::doFrame(Renderer::FrameContext& ctx) noexcept {
  auto& recorder = ctx.cmdRecorder;
  recorder.recordCmd(ctx.data, BeginRenderingArgs{});
  recorder.recordCmd(ctx.data, SetViewportArgs{});
  recorder.recordCmd(ctx.data, SetScissorArgs{});
  recorder.recordCmd(ctx.data, BindPipelineArgs{});
  recorder.recordCmd(ctx.data, EndRenderingArgs{});
}

void Renderer::endFrame(Renderer::FrameContext& ctx) noexcept {
  // 2. end frame
  mBackend->endFrame(ctx.cmdRecorder, ctx.data);
}
}  // namespace pd