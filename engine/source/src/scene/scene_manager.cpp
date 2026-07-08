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
  auto unloadResult = unloadScene();
  if (!unloadResult) {
    LOG_ERROR("scene unload error: {}", unloadResult.error().msg);
  }
  mInitialized = false;
}

Result<void> SceneManager::loadScene() noexcept {
  mResourceManager->clearAll();
  auto sceneLoadResult = mSceneDescriptor->loadScene(*this);
  if (!sceneLoadResult) {
    LOG_ERROR("scene cannot be loaded {}", sceneLoadResult.error().msg);
    return sceneLoadResult;
  }
  mResourceManager->loadAll();
  return {};
}

void SceneManager::updateScene() noexcept {}

Result<void> SceneManager::unloadScene() noexcept {
  auto unloadResult = mSceneDescriptor->unloadScene();
  if (!unloadResult) {
    LOG_ERROR("scene unload failed {}", unloadResult.error().msg);
  }
  mResourceManager->clearAll();
  //   mWorld.clear();
  return {};
}
}  // namespace pd