#pragma once

#include <concepts>

namespace pd {
class SceneManager;
class Scene;
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
  Result<void> loadScene(SceneManager& sceneManager) noexcept;

  /**
   * @brief 帧内更新场景数据
   * @todo not implemented
   *
   */
  void updateScene() noexcept;

  /**
   * @brief 卸载场景
   *
   * @return SceneResult<void>
   */
  Result<void> unloadScene() noexcept;

 private:
  virtual Result<void> onLoadScene(SceneManager& sceneManager) noexcept = 0;
  virtual void onUpdateScene() noexcept = 0;
  virtual Result<void> onUnloadScene() noexcept = 0;
};

// 只允许处理继承了此对象的场景描述类
template <typename T>
concept BaseOfSceneDescriptor = std::derived_from<T, SceneDescriptor>;
}  // namespace pd