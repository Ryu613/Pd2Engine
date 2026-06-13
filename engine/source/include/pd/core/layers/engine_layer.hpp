#pragma once

#include "pd/core/layer.hpp"
#include "pd/platform/window/window.hpp"
#include "pd/backend/backend.hpp"

namespace pd {
class EngineLayer : public Layer {
 public:
  explicit EngineLayer(Window* window) noexcept;
  virtual ~EngineLayer() = default;
  MOVABLE_ONLY(EngineLayer);

 protected:
  LayerResult<void> onAttached() noexcept override;
  LayerResult<void> onDetached() noexcept override;
  void onUpdate() noexcept override;

 private:
  Window* mWindow = nullptr;
  std::unique_ptr<Backend> mBackend;
};
}  // namespace pd