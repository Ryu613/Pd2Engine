#pragma once

#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset_manager.hpp"

namespace pd {
class SceneDescriptor;
// 只允许处理继承了此对象的场景描述类
template <typename T>
concept DerivedSceneDescriptor = std::derived_from<T, SceneDescriptor>;
class SceneManager {
 public:
  explicit SceneManager(ResourceManager* rscMgr, AssetManager* assetMgr);
  ~SceneManager();
  DELETE_COPY_MOVE(SceneManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  template <DerivedSceneDescriptor T, typename... Args>
  Result<void> initScene(Args&&... args) noexcept;

  Result<void> loadScene() noexcept;
  void updateScene(float delta) noexcept;
  Result<void> unloadScene() noexcept;

 private:
  ResourceManager* mRscMgr = nullptr;
  AssetManager* mAssetMgr = nullptr;
  std::unique_ptr<SceneDescriptor> mSceneDescriptor;
};

template <DerivedSceneDescriptor T, typename... Args>
inline Result<void> SceneManager::initScene(Args&&... args) noexcept {
  mSceneDescriptor = std::make_unique<T>(std::forward<Args>(args)...);
  return {};
}
}  // namespace pd