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
  auto FrameData = beginFrame();
  doFrame(FrameData);
  endFrame(FrameData);
}

FrameData Renderer::beginFrame() noexcept {
  // 1. begin new frame
  return mBackend->beginFrame();
  // 1. acquire backend next frame
  // 1.1 recreate swapchain if needed
}

void Renderer::doFrame(FrameData& data) noexcept {
  // 1. create render graph
  // 1.1 add render pass
  // 1.2 compile render graph
  // 1.3 execute render graph
}

void Renderer::endFrame(FrameData& data) noexcept {
  // 1. submit commands
  // 2. present frame
  // 3. end frame
  mBackend->endFrame(data);
  // 4. update frame data
}
}  // namespace pd