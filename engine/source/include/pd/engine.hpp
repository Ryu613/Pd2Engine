#pragma once

#include "pd/core/globals.hpp"
#include "pd/core/allocators.hpp"
#include "pd/core/entity_manager.hpp"
#include "pd/platform/platform.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/rendering/renderer.hpp"
#include "pd/scene/scene_descriptor.hpp"
#include "pd/scene/scene.hpp"
#include "pd/scene/manager/transform_manager.hpp"
#include "pd/scene/manager/renderable_manager.hpp"
#include "pd/scene/manager/camera_manager.hpp"
#include "pd/scene/manager/light_manager.hpp"
#include "pd/core/layer.hpp"

namespace pd {
/**
 * 引擎全局入口，管理和协调各个子系统的生命周期和运行状态，可视为一个薄的异构容器，不参与具体逻辑
 */
class Engine {
 public:
  struct Config {
    std::string appName = global::DefaultWindowTitle;
    uint32_t windowWidth = global::DefaultWindowWidth;
    uint32_t windowHeight = global::DefaultWindowHeight;
    bool enableDebug = true;
  };
  enum class Error : uint8_t {
    InitializeFailed,
    RunFailed,
    LayerNotFound,
  };
  template <typename T>
  using EngineResult = Result<T, Error>;

  explicit Engine(Config config) noexcept;
  ~Engine() noexcept;

  NO_COPY_MOVE(Engine);

  EngineResult<void> initialize() noexcept;
  void shutdown() noexcept;

  //   template <DerivedSceneDescriptor T, typename... Args>
  //   std::expected<void, EngineError> loadScene(Args&&... args) noexcept {
  //     mSceneDescriptor = std::make_unique<T>(std::forward<Args>(args)...);
  //     mSceneDescriptor->onLoad(*mAssetManager, mEntityManager, mTransformManager,
  //                              mCameraManager, mScene);
  //   }
  /**
   * @brief 创建界面层
   *
   * @tparam TLayer 要求派生自Layer
   * @tparam Args Layer构造函数参数
   * @param args
   * @return EngineResult<void>
   */
  template <BaseOfLayer TLayer, typename... Args>
  EngineResult<void> createLayer(Args... args) {
    auto layer = std::make_unique<TLayer>(std::forward<Args>(args)...);
    return attachLayer(std::move(layer));
  }

  EngineResult<Layer*> attachLayer(std::unique_ptr<Layer>&& layer) noexcept;

  EngineResult<std::unique_ptr<Layer>> detachLayer(Layer* layer) noexcept;

  EngineResult<void> run() noexcept;

 private:
  Config mConfig;
  bool mInitialized = false;

  std::vector<std::unique_ptr<Layer>> mLayers;

  HeapAllocator mArena;
  ResourceManager mResourceManager;
  EntityManager mEntityManager;
  // scene property managers
  TransformManager mTransformManager;
  RenderableManager mRenderableManager;
  CameraManager mCameraManager;
  LightManager mLightManager;

  std::unique_ptr<Platform> mPlatform;
  std::unique_ptr<AssetManager> mAssetManager;
  std::unique_ptr<Renderer> mRenderer;
  // 相当于world
  Scene mScene;

  std::unique_ptr<SceneDescriptor> mSceneDescriptor;
};
}  // namespace pd