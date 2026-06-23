#include "pd/scene/scene_descriptor.hpp"

#include "pd/scene/scene_manager.hpp"

namespace pd {

Result<void, Error::Scene> SceneDescriptor::loadScene(
    SceneManager& sceneManager) noexcept {
  log::info("start loading scene");
  auto loadResult = onLoadScene(sceneManager);
  if (!loadResult) {
    log::error("scene load failed!", GetEnumName(loadResult.error()));
    return loadResult;
  }
  return {};
}

void SceneDescriptor::updateScene() noexcept {}

Result<void, Error::Scene> SceneDescriptor::unloadScene() noexcept {
  // TODO(author): not implemented
  return {};
}
}  // namespace pd