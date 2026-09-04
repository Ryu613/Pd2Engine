#include "pd/scene/scene_manager.hpp"

namespace pd {
SceneManager::SceneManager(ResourceManager* rscMgr, AssetManager* assetMgr)
    : mRscMgr(rscMgr),
      mAssetMgr(assetMgr) {}

SceneManager::~SceneManager() {}

Result<void> SceneManager::init() noexcept { return {}; }

Result<void> SceneManager::destroy() noexcept { return {}; }
}  // namespace pd