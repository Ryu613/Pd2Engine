#pragma once

namespace pd {
class SceneManager;
class SceneDescriptor {
 public:
  SceneDescriptor() noexcept = default;
  virtual ~SceneDescriptor() = default;
  DELETE_COPY(SceneDescriptor);
  DEFAULT_MOVABLE(SceneDescriptor);

  /**
   * @brief 在渲染前需要准备好的资产，场景, 视窗
   */
  Result<void> loadScene(SceneManager& sceneManager) noexcept;

  /**
   * @brief 帧内更新场景数据
   *
   */
  void updateScene(float deltaTime) noexcept;

  /**
   * @brief 卸载场景
   *
   * @return Result<void>
   */
  Result<void> unloadScene(SceneManager& sceneManager) noexcept;

 private:
  virtual Result<void> onLoadScene(SceneManager& sceneManager) noexcept = 0;
  virtual void onUpdateScene(float deltaTime) noexcept = 0;
  virtual Result<void> onUnloadScene(SceneManager& sceneManager) noexcept = 0;
};
}  // namespace pd