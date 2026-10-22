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
 * 引擎全局入口，管理和协调各个子系统的生命周期和运行状态，可视为一个薄的异构容器，
 * 子系统作为其成员，不参与具体子系统的内部逻辑
 * 若出现未捕获的异常，应直接终止程序，若异常可被处理，应该用Result获取并处理
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
   * @brief 加载场景并运行
   *
   * @return Result<void>
   */
  template <DerivedSceneDescriptor T, typename... Args>
  Result<void> run(Args&&... args) noexcept;

  /**
   * @brief 请求停止运行
   *
   * @return Result<void>
   */
  Result<void> requestStop() noexcept;

  /**
   * @brief 获取引擎当前状态
   */
  EngineStatus status() const noexcept;

 private:
  EngineConfig mConfig;
  EngineStatus mStatus;
  // common services
  GlobalAllocator mArena;
  // sub systems
  Platform mPlatform;
  Backend mBackend;
  ResourceManager mResourceManager;
  AssetManager mAssetManager;
  SceneManager mSceneManager;
  Renderer mRenderer;

  Result<void> runImpl() noexcept;
  Result<void> stop() noexcept;
  void loop() noexcept;
  void tick() noexcept;
};

template <DerivedSceneDescriptor T, typename... Args>
inline Result<void> Engine::run(Args&&... args) noexcept {
  PD_ASSERT(mStatus == EngineStatus::Initialized);
  // construct scene descriptor
  if (auto res = mSceneManager.initScene<T>(std::forward<Args>(args)...); !res) {
    return res;
  }
  if (auto res = runImpl(); !res) {
    return res;
  }
  return {};
}
}  // namespace pd