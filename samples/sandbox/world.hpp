#pragma once

#include "pd/scene/scene_descriptor.hpp"

namespace sandbox {
using namespace pd;
class WorldScene : public SceneDescriptor {
 private:
  Result<void> onLoadScene(SceneManager& sceneManager) noexcept override { return {}; };

  void onUpdateScene() noexcept override {}

  Result<void> onUnloadScene() noexcept override { return {}; }
};
}  // namespace sandbox