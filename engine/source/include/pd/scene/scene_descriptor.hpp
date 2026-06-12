#pragma once

#include <concepts>

#include "pd/scene/scene.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/scene/manager/transform_manager.hpp"
#include "pd/scene/manager/camera_manager.hpp"

namespace pd {
/**
 * @brief
 * 负责对整个场景进行描述，包括需要哪些资产，如何使用这些资产布置场景，窗口怎么"看"场景等
 *
 * 用户使用时应继承此类并重写两个函数，在引擎启动前，需要调用loadScene()来执行场景构建
 * 引擎停止时，会自行调用onUnload()来执行自定义逻辑，包括但不限于手动卸载资源等
 */
class SceneDescriptor {
 public:
  SceneDescriptor() noexcept = default;
  virtual ~SceneDescriptor() = default;

  MOVABLE_ONLY(SceneDescriptor);
  /**
   * @brief 在渲染前需要准备好的资产，场景, 视窗
   */
  virtual void onLoad(AssetManager& assetMgr, EntityManager& entityMgr,
                      TransformManager& transformMgr, CameraManager& cameraMgr,
                      Scene& scene) = 0;

  /**
   * @brief 用于在渲染后处理自定义逻辑和清理此场景资源
   */
  virtual void onUnload(Scene& scene) = 0;
};

// 只允许处理继承了此对象的场景描述类
template <typename T>
concept DerivedSceneDescriptor = std::derived_from<T, SceneDescriptor>;
}  // namespace pd