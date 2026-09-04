#include "pd/scene/scene_manager.hpp"

#include "pd/scene/scene_descriptor.hpp"

namespace pd {
SceneManager::SceneManager(ResourceManager* rscMgr, AssetManager* assetMgr)
    : mRscMgr(rscMgr),
      mAssetMgr(assetMgr) {}

SceneManager::~SceneManager() {}

Result<void> SceneManager::init() noexcept { return {}; }

Result<void> SceneManager::destroy() noexcept { return {}; }

Result<void> SceneManager::loadScene() noexcept {
  if (auto res = mSceneDescriptor->loadScene(*this); !res) {
    return res;
  }
  return {};
}

void SceneManager::updateScene(float delta) noexcept {}

Result<void> SceneManager::unloadScene() noexcept {
  if (auto res = mSceneDescriptor->unloadScene(*this); !res) {
    return res;
  }
  return {};
}
}  // namespace pd