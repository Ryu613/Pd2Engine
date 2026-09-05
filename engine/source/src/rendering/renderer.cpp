#include "pd/rendering/renderer.hpp"

namespace pd {
Renderer::Renderer(SceneManager* sceneMgr, Backend* backend)
    : mSceneManager(sceneMgr),
      mBackend(backend) {}

Renderer::~Renderer() {}

Result<void> Renderer::init() noexcept {
  // 1. init backend frames
  // 3. init render graph
  // 4. create swapchain
  return {};
}

Result<void> Renderer::destroy() noexcept {
  // 1. destroy swapchain
  // 2. destroy render graph
  // 3. destroy in-frame data
  // 4. destroy frame context
  return {};
}

void Renderer::renderFrame() noexcept {
  beginFrame();
  doFrame();
  endFrame();
}

void Renderer::beginFrame() noexcept {
  // 1. get current frame
  // 1. acquire backend next frame
  // 1.1 recreate swapchain if needed
}

void Renderer::doFrame() noexcept {
  // 1. create render graph
  // 1.1 add render pass
  // 1.2 compile render graph
  // 1.3 execute render graph
}

void Renderer::endFrame() noexcept {
  // 1. submit commands
  // 2. present frame
  // 3. end frame
  // 4. update frame data
}
}  // namespace pd