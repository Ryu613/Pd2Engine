#pragma once

#include "pd/rendering/layer/layer.hpp"

namespace pd {
class GuiLayer : public ILayer {
 public:
  GuiLayer() noexcept;
  virtual ~GuiLayer() = default;
  MOVABLE_ONLY(GuiLayer);

 protected:
  LayerResult<void> onAttached() noexcept override;
  LayerResult<void> onDetached() noexcept override;
  void onUpdate() noexcept override;
};
}  // namespace pd