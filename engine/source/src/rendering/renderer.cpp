#include "pd/rendering/renderer.hpp"

#include "pd/scene/scene.hpp"

namespace pd {
Renderer::Renderer(SceneManager* sceneMgr, Backend* backend)
    : mSceneManager(sceneMgr),
      mBackend(backend),
      mMaterialManager(backend) {}

Renderer::~Renderer() {}

Result<void> Renderer::init() noexcept {
  LOG_INFO("renderer init()");
  if (auto res = mMaterialManager.init(); !res) {
    return res;
  }
  return {};
}

Result<void> Renderer::destroy() noexcept {
  LOG_INFO("renderer destroy()");
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
  // todo: update camera
  // for each scene entity renderables
  const auto& sceneData = mSceneManager->getSceneData();
  for (size_t i = 0; i < sceneData.size(); ++i) {
    const auto& renderable = sceneData[i];
    const auto pipeline = mMaterialManager.getPipeline(renderable.materialInstance);
    recorder.addCmd(BeginRenderingArgs{});
    recorder.addCmd(SetViewportArgs{});
    recorder.addCmd(SetScissorArgs{});
    recorder.addCmd(BindPipelineArgs{
        .vertexBuffer = renderable.vertexBuffer,
        .vertexBufferOffset = renderable.vertexBufferOffset,
        .indexBuffer = renderable.indexBuffer,
        .indexBufferOffset = renderable.indexBufferOffset,
        .pipeline = pipeline,
    });
    recorder.addCmd(DrawIndexedArgs{
        .indexCount = renderable.indexCount,
        .instanceCount = 1,
        .firstIndex = 0,
        .vertexOffset = 0,
        .firstInstance = 0,
    });
    recorder.addCmd(EndRenderingArgs{});
  }
}

void Renderer::endFrame(Renderer::FrameContext& ctx) noexcept {
  // 2. end frame
  mBackend->endFrame(ctx.cmdRecorder);
}
}  // namespace pd