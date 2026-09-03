#pragma once

#include "pd/core/allocators.hpp"
#include "pd/platform/platform.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/scene/scene_manager.hpp"
#include "pd/backend/backend.hpp"
#include "pd/rendering/renderer.hpp"
#include "pd/rendering/layer/layer.hpp"

namespace pd {
/**
 * 引擎全局入口，管理和协调各个子系统的生命周期和运行状态，可视为一个薄的异构容器，子系统作为其成员，不参与具体子系统的内部逻辑
 */
class Engine {
 public:
  explicit Engine(EngineConfig config) noexcept;
  ~Engine() noexcept;

  DELETE_COPY_MOVE(Engine);

  /**
   * @brief 引擎初始化
   *
   * @return Result<void>
   */
  Result<void> initialize() noexcept;

  /**
   * @brief 关闭引擎，应在销毁前调用
   *
   */
  void shutdown() noexcept;

  /**
   * @brief 通过场景描述器加载场景数据
   *
   * @tparam T 场景描述派生类型
   * @tparam Args 构造参数类型
   * @param args 构造参数
   * @return Result<void> 场景加载结果
   */
  template <BaseOfSceneDescriptor T, typename... Args>
  Result<void> loadScene(Args&&... args) noexcept {
    return mSceneManager.initializeScene<T>(std::forward<Args>(args)...);
  }

  /**
   * @brief 创建界面层, 用于在渲染画面之上显示自定义界面，由引擎构造layer, 支持运行时创建和销毁
   *
   * @tparam TLayer 要求派生自Layer接口
   * @tparam Args Layer构造函数参数
   * @param args
   * @return EngineResult<void>
   */
  //   template <BaseOfLayer TLayer, typename... Args>
  //   Result<void> createLayer(Args... args) noexcept {
  //     auto layer = std::make_unique<TLayer>(std::forward<Args>(args)...);
  //     return attachLayer(std::move(layer));
  //   }

  /**
   * @brief 把界面层附着到引擎中,外部可派生layer来自定义界面逻辑，拥有权转入,外部构造后传入
   * @see rendering/layer
   *
   * @param layer
   * @return EngineResult<Layer*>
   */
  //   Result<void> attachLayer(std::unique_ptr<ILayer>&& layer) noexcept;

  /**
   * @brief 把界面层从引擎中拔下，拥有权转出
   *
   * @param layer
   * @return EngineResult<std::unique_ptr<Layer>>
   */
  //   Result<std::unique_ptr<ILayer>> detachLayer(ILayer* layer) noexcept;

  /**
   * @brief 开始运行
   *
   * @return EngineResult<void>
   */
  Result<void> run() noexcept;

  /**
   * @brief 停止运行
   *
   * @return Result<void>
   */
  Result<void> stop() noexcept;

 private:
  EngineConfig mConfig;
  bool mInitialized = false;

  GlobalAllocator mArena;

  IPlatform mPlatform;
  ResourceManager mResourceManager;
  AssetManager mAssetManager;
  SceneManager mSceneManager;
  IBackend mBackend;
  Renderer mRenderer;
  // FIXME: need update for this
  //   std::vector<std::unique_ptr<ILayer>> mLayers;
};
}  // namespace pd