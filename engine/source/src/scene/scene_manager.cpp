#include "pd/scene/scene_manager.hpp"

#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset_manager.hpp"

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

void SceneManager::updateScene(float deltaTime) noexcept {
  // TODO(ryu613): update view
  // TODO(ryu613): scene culling
  // TODO(ryu613): prepare scene resources cache
  // shortcut: not culled
  updateRenderables(mWorld.mEntities);
  // TODO(ryu613): update rscmgr's references
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
  // shortcut for testing
  for (const auto& [entity, transform, assetId] :
       mEntityManager.viewOf<Transform, Asset::IdType>().each()) {
    auto* Asset = mAssetManager.getAsset(assetId);
    auto* meshResource = mResourceManager->getResource<MeshResource_t>(meshHandle);
    auto& primitives = meshResource->primitives();
    for (int i = 0; i < primitives.size(); ++i) {
      Renderable ren{
          .transform = transform.toLocalMatrix(),
          .vertexBuffer = meshResource->getVertexBuffer(),
          .indexBuffer = meshResource->getIndexBuffer(),
          .vertexOffset = static_cast<u32>(sizeof(Vertex) * i),
          .indexOffset = static_cast<u32>(sizeof(u32) * i),
      };
      mRenderables.emplace_back(ren);
    }
  }
}
}  // namespace pd