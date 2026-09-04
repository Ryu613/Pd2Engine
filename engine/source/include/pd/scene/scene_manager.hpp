#pragma once

#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset_manager.hpp"

namespace pd {
class SceneManager {
 public:
  explicit SceneManager(ResourceManager* rscMgr, AssetManager* assetMgr);
  ~SceneManager();
  DELETE_COPY_MOVE(SceneManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

 private:
  ResourceManager* mRscMgr = nullptr;
  AssetManager* mAssetMgr = nullptr;
};
}  // namespace pd