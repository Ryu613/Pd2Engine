#pragma once

#include "pd/rendering/layer/layer.hpp"

namespace pd {
class DebugLayer : public ILayer {
 public:
  DebugLayer() noexcept;
  virtual ~DebugLayer() = default;
  MOVABLE_ONLY(DebugLayer);

 protected:
  Result<void> onAttached() noexcept override;
  Result<void> onDetached() noexcept override;
  void onUpdate() noexcept override;
};
}  // namespace pd