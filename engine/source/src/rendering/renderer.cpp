#include "pd/rendering/renderer.hpp"

namespace pd {
Renderer::Renderer(SceneManager* sceneMgr, Backend* backend)
    : mSceneManager(sceneMgr),
      mBackend(backend) {}

Renderer::~Renderer() {}

Result<void> Renderer::init() noexcept {
    return {};
}

Result<void> Renderer::destroy() noexcept {
    return {};
}
}  // namespace pd