#pragma once

#include <concepts>

#include "pd/asset/asset_manager.hpp"

namespace pd {
class AssetManager;
class TransformManager;
class Scene;
class View;
/**
 * @brief
 * 负责对整个场景进行描述，包括需要哪些资产，如何使用这些资产布置场景，窗口怎么"看"场景等
 *
 * 用户使用时应继承此类并重写三个函数，在引擎启动后，渲染前会按照描述真正开始加载和处理过程
 */
class SceneDescription {
 public:
  virtual ~SceneDescription() = default;

  MOVABLE_ONLY(SceneDescription);
  /**
   * @brief 在渲染前需要准备好的资产，场景, 视窗
   */
  virtual void preRender(AssetManager& assetMgr, TransformManager& transformMgr,
                         Scene& scene, View& view) = 0;

  /**
   * @brief 用于在渲染后处理自定义逻辑和清理此场景资源
   */
  virtual void postRender(AssetManager& resourceMgr) = 0;
};

// 只允许处理继承了此对象的场景描述类
template <typename T>
concept DerivedSceneDescription = std::derived_from<T, SceneDescription>;
}  // namespace pd