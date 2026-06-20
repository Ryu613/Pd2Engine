#pragma once

#include "pd/core/error.hpp"
#include "pd/scene/scene.hpp"
#include "pd/scene/scene_descriptor.hpp"
#include "pd/scene/manager/entity_manager.hpp"
#include "pd/scene/manager/transform_manager.hpp"
#include "pd/scene/manager/light_manager.hpp"
#include "pd/scene/manager/camera_manager.hpp"

namespace pd {
class AssetManager;
class SceneManager {
 public:
  SceneManager() noexcept = default;
  ~SceneManager() = default;
  explicit SceneManager(AssetManager* assetMgr) noexcept;
  MOVABLE_ONLY(SceneManager);

  template <typename T>
  using SceneResult = Result<T, EngineError>;

  template <BaseOfSceneDescriptor T, typename... Args>
  SceneResult<void> initializeScene(Args&&... args) noexcept {
    auto unloadResult = unloadScene();
    if (!unloadResult) {
      return unloadResult;
    }
    mSceneDescriptor = std::make_unique<T>(std::forward<Args>(args)...);
  }

  SceneResult<void> loadScene() noexcept {
    mSceneDescriptor->onLoad(*mAssetManager, mEntityManager, mTransformManager,
                             mLightManager, mCameraManager, mWorld);
    return {};
  }

  void updateScene() noexcept {
    // mSceneDescriptor->unUpdate(AssetManager &assetMgr, EntityManager &entityMgr,
    // TransformManager &transformMgr, CameraManager &cameraMgr, Scene &scene)
  }

  SceneResult<void> unloadScene() noexcept {
    mSceneDescriptor->onUnload();
    // mWorld.clear();
    return {};
  }

 private:
  Scene mWorld;
  AssetManager* mAssetManager = nullptr;
  EntityManager mEntityManager;
  TransformManager mTransformManager;
  LightManager mLightManager;
  CameraManager mCameraManager;
  std::unique_ptr<SceneDescriptor> mSceneDescriptor;
};
}  // namespace pd