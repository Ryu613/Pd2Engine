#include "pd/scene/scene_manager.hpp"

#include "pd/resource/resource_manager.hpp"

namespace pd {

void SceneManager::initialize(AssetManager* assetMgr, ResourceManager* rscMgr) noexcept {
  if (mInitialized) {
    return;
  }
  mAssetManager = assetMgr;
  mResourceManager = rscMgr;

  mInitialized = true;
}

SceneManager::~SceneManager() { destroy(); }

void SceneManager::destroy() noexcept {
  if (!mInitialized) {
    return;
  }
  unloadScene();
  mInitialized = false;
}

Result<void, Error::Scene> SceneManager::loadScene() noexcept {
  mResourceManager->clearAll();
  auto sceneLoadResult = mSceneDescriptor->loadScene(*this);
  if (!sceneLoadResult) {
    log::error("scene cannot be loaded!", sceneLoadResult.error());
    return sceneLoadResult;
  }
  mResourceManager->loadAll();
  return {};
}

void SceneManager::updateScene() noexcept {}

Result<void, Error::Scene> SceneManager::unloadScene() noexcept {
  auto unloadResult = mSceneDescriptor->unloadScene();
  if (!unloadResult) {
    log::error("scene unload failed!", unloadResult.error());
  }
  mResourceManager->clearAll();
  //   mWorld.clear();
  return {};
}
}  // namespace pd