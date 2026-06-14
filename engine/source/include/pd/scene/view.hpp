#pragma once

#include "pd/scene/entity.hpp"

namespace pd {
class View {
 public:
  struct Viewport {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
  };
  View() noexcept = default;
  ~View() = default;
  MOVABLE_ONLY(View);

  void setMainCamera(Entity cameraEntity) noexcept { mMainCamera = cameraEntity; }

 private:
  Viewport mViewport{};
  Entity mMainCamera{};
};
}  // namespace pd
