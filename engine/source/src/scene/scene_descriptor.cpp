#include "pd/scene/scene_descriptor.hpp"

#include "pd/scene/scene_manager.hpp"

namespace pd {

Result<void> SceneDescriptor::loadScene(
    SceneManager& sceneManager) noexcept {
  LOG_INFO("start loading scene");
  auto loadResult = onLoadScene(sceneManager);
  if (!loadResult) {
    LOG_ERROR("scene load failed: {}", default_msg(loadResult.error().code));
    return loadResult;
  }
  LOG_INFO("load scene complete!");
  return {};
}

void SceneDescriptor::updateScene() noexcept {}

Result<void> SceneDescriptor::unloadScene() noexcept {
  // TODO(author): not implemented
  return {};
}
}  // namespace pd