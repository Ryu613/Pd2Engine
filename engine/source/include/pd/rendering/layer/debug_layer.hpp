#pragma once

#include "pd/core/layer.hpp"

namespace pd {
class DebugLayer : public Layer {
 public:
  DebugLayer() noexcept;
  virtual ~DebugLayer() = default;
  MOVABLE_ONLY(DebugLayer);

 protected:
  LayerResult<void> onAttached() noexcept override;
  LayerResult<void> onDetached() noexcept override;
  void onUpdate() noexcept override;
};
}  // namespace pd