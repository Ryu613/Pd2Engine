#include "pd/scene/scene_manager.hpp"

namespace pd {
SceneManager::SceneManager(AssetManager* assetMgr, ResourceManager* rscMgr) noexcept
    : mAssetManager(assetMgr),
      mResourceManager(rscMgr) {}

Result<void, Error::Scene> SceneManager::loadScene() noexcept {
  auto sceneLoadResult = mSceneDescriptor->loadScene(*this);
  if (!sceneLoadResult) {
    log::error("scene cannot be loaded!", sceneLoadResult.error());
    return sceneLoadResult;
  }
  return {};
}

void SceneManager::updateScene() noexcept {}

Result<void, Error::Scene> SceneManager::unloadScene() noexcept {
  auto unloadResult = mSceneDescriptor->unloadScene();
  if (!unloadResult) {
    log::error("scene unload failed!", unloadResult.error());
    return unloadResult;
  }
  // mWorld.clear();
  return {};
}
}  // namespace pd