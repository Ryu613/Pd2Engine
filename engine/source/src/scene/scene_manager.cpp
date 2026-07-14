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
  mRenderables.clear();
  auto sceneLoadResult = mSceneDescriptor->loadScene(*this);
  if (!sceneLoadResult) {
    LOG_ERROR("scene cannot be loaded {}", sceneLoadResult.error().msg);
    return sceneLoadResult;
  }
  return {};
}

void SceneManager::updateScene() noexcept {
  // TODO(author): update view
  // TODO(author): scene culling
  // TODO(author): prepare scene resources cache
  // shortcut: not culled
  updateRenderables(mWorld.mEntities);
  // TODO(author): update rscmgr's references
  // rscmgr gc
  //   mResourceManager->gc();
  // load remained resources if not loaded
  mResourceManager->loadAll();
}

Result<void> SceneManager::unloadScene() noexcept {
  auto unloadResult = mSceneDescriptor->unloadScene();
  if (!unloadResult) {
    LOG_ERROR("scene unload failed {}", unloadResult.error().msg);
  }
  mRenderables.clear();
  //   mWorld.clear();
  mResourceManager->clearAll();
  return {};
}

void SceneManager::updateRenderables(const std::vector<Entity>& culledEntities) noexcept {
  mRenderables.clear();
  mRenderables.shrink_to_fit();
  mRenderables.reserve(culledEntities.size());
  for (const auto& [entity, transform, meshHandle] :
       mEntityManager.viewOf<Transform, MeshHandle>().each()) {
    auto* meshResource = mResourceManager->getResource<MeshResource_t>(meshHandle);
    mRenderables.emplace_back(transform, meshResource);
  }
}
}  // namespace pd