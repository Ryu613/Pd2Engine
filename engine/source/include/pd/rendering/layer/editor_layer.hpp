#pragma once

#include "pd/core/layer.hpp"

namespace pd {
class EditorLayer : public Layer {
 public:
  EditorLayer() noexcept;
  virtual ~EditorLayer() = default;
  MOVABLE_ONLY(EditorLayer);

 protected:
  LayerResult<void> onAttached() noexcept override;
  LayerResult<void> onDetached() noexcept override;
  void onUpdate() noexcept override;
};
}  // namespace pd