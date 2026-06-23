#pragma once

#include "pd/rendering/layer/layer.hpp"

namespace pd {
class DebugLayer : public ILayer {
 public:
  DebugLayer() noexcept;
  virtual ~DebugLayer() = default;
  MOVABLE_ONLY(DebugLayer);

 protected:
  Result<void, Error::Layer> onAttached() noexcept override;
  Result<void, Error::Layer> onDetached() noexcept override;
  void onUpdate() noexcept override;
};
}  // namespace pd