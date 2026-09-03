#pragma once

#include "pd/scene/scene.hpp"
#include "pd/scene/scene_descriptor.hpp"
#include "pd/scene/component/components.hpp"
#include "pd/scene/manager/entity_manager.hpp"
// #include "pd/scene/manager/renderable_manager.hpp"
// #include "pd/scene/manager/transform_manager.hpp"
// #include "pd/scene/manager/light_manager.hpp"
// #include "pd/scene/manager/camera_manager.hpp"

namespace pd {
class AssetManager;
class ResourceManager;
struct Renderable {
  math::mat4 transform;
  HwHandle<Buffer_t> vertexBuffer;
  HwHandle<Buffer_t> indexBuffer;
  u32 vertexOffset = 0;
  u32 indexOffset = 0;
};
class SceneManager {
 public:
  SceneManager() noexcept = default;
  ~SceneManager();
  explicit SceneManager(AssetManager* assetMgr, ResourceManager* rscMgr) noexcept;
  DELETE_COPY_MOVE(SceneManager);

  void initialize(AssetManager* assetMgr, ResourceManager* rscMgr) noexcept;

  void destroy() noexcept;

  template <BaseOfSceneDescriptor T, typename... Args>
  Result<void> initializeScene(Args&&... args) noexcept {
    mSceneDescriptor = std::make_unique<T>(std::forward<Args>(args)...);
    return {};
  }

  Result<void> loadScene() noexcept;

  void updateScene(float deltaTime) noexcept;

  Result<void> unloadScene() noexcept;

  AssetManager& getAssetManager() noexcept { return *mAssetManager; }

  ResourceManager& getResourceManager() noexcept { return *mResourceManager; }

  EntityManager& getEntityManager() noexcept { return mEntityManager; }

  Scene& getScene() noexcept { return mWorld; }

  std::vector<Renderable>& getRenderables() noexcept { return mRenderables; }

  //   RenderableManager& getRenderableManager() noexcept { return mRenderableManager; }

  //   TransformManager& getTransformManager() noexcept { return mTransformManager; }

  //   LightManager& getLightManager() noexcept { return mLightManager; }

  //   CameraManager& getCameraManager() noexcept { return mCameraManager; }

 private:
  Scene mWorld;
  std::vector<Renderable> mRenderables;
  EntityManager mEntityManager;
  AssetManager* mAssetManager = nullptr;
  ResourceManager* mResourceManager = nullptr;
  //   TransformManager mTransformManager;
  //   RenderableManager mRenderableManager;
  //   LightManager mLightManager;
  //   CameraManager mCameraManager;
  std::unique_ptr<SceneDescriptor> mSceneDescriptor;
  bool mInitialized = false;

  void updateRenderables(const std::vector<Entity>& culledEntities) noexcept;
};
}  // namespace pd