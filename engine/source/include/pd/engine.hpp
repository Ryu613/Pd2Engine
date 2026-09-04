#pragma once

#include "pd/core/allocators.hpp"
#include "pd/platform/platform.hpp"
#include "pd/backend/backend.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/scene/scene_manager.hpp"
#include "pd/rendering/renderer.hpp"

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
  Result<void> shutdown() noexcept;

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
  // common services
  GlobalAllocator mArena;
  // sub systems
  Platform mPlatform;
  Backend mBackend;
  ResourceManager mResourceManager;
  AssetManager mAssetManager;
  SceneManager mSceneManager;
  Renderer mRenderer;
};
}  // namespace pd