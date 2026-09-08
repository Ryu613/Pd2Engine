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
  auto frameData = beginFrame();
  doFrame(frameData);
  endFrame(frameData);
}

FrameData Renderer::beginFrame() noexcept {
  // 1. begin new frame
  return mBackend->beginFrame();
}

void Renderer::doFrame(FrameData& data) noexcept {
  auto& recorder = data.cmdRecorder;
  recorder.recordCmd(DrawCmdArgs{
      .vertexCount = 1,
  });
  // 1. create render graph
  // 1.1 add render pass
  // 1.2 compile render graph
  // 1.3 execute render graph
}

void Renderer::endFrame(FrameData& data) noexcept {
  // 1. submit commands
  data.cmdRecorder.submit();
  // 2. end frame
  mBackend->endFrame(data);
}
}  // namespace pd