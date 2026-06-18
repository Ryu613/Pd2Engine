#pragma once

#include "pd/core/allocators.hpp"
#include "pd/core/config.hpp"
#include "pd/core/error.hpp"
#include "pd/platform/platform.hpp"
#include "pd/backend/backend.hpp"
#include "pd/rendering/layer/layer.hpp"

namespace pd {
/**
 * 引擎全局入口，管理和协调各个子系统的生命周期和运行状态，可视为一个薄的异构容器，不参与具体逻辑
 */
class Engine {
 public:
  using Config = EngineConfig;
  using Error = EngineError;

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
   * @brief 创建界面层, 支持运行时创建和销毁
   *
   * @tparam TLayer 要求派生自Layer接口
   * @tparam Args Layer构造函数参数
   * @param args
   * @return EngineResult<void>
   */
  template <BaseOfLayer TLayer, typename... Args>
  EngineResult<void> createLayer(Args... args) noexcept {
    auto layer = std::make_unique<TLayer>(std::forward<Args>(args)...);
    return attachLayer(std::move(layer));
  }

  /**
   * @brief 把界面层附着到引擎中,外部可派生layer来自定义界面逻辑，拥有权转入
   * @see rendering/layer
   *
   * @param layer
   * @return EngineResult<Layer*>
   */
  EngineResult<ILayer*> attachLayer(std::unique_ptr<ILayer>&& layer) noexcept;

  /**
   * @brief 把界面层从引擎中拔下，拥有权转出
   *
   * @param layer
   * @return EngineResult<std::unique_ptr<Layer>>
   */
  EngineResult<std::unique_ptr<ILayer>> detachLayer(ILayer* layer) noexcept;

  /**
   * @brief 开始运行
   *
   * @return EngineResult<void>
   */
  EngineResult<void> run() noexcept;

 private:
  Config mConfig;
  bool mInitialized = false;

  HeapAllocator mArena;

  std::unique_ptr<IPlatform> mPlatform;
  std::unique_ptr<IBackend> mBackend;

  std::vector<std::unique_ptr<ILayer>> mLayers;
  //   Renderer mRenderer;

  //   ResourceManager mResourceManager;
  //   EntityManager mEntityManager;
  //   // scene property managers
  //   TransformManager mTransformManager;
  //   RenderableManager mRenderableManager;
  //   CameraManager mCameraManager;
  //   LightManager mLightManager;

  //   std::unique_ptr<AssetManager> mAssetManager;
  //   std::unique_ptr<Renderer> mRenderer;
  // 相当于world
  //   Scene mScene;

  //   std::unique_ptr<SceneDescriptor> mSceneDescriptor;
};
}  // namespace pd