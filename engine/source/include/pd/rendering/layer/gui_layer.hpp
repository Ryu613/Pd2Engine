#pragma once

#include "pd/rendering/layer/layer.hpp"

namespace pd {
class GuiLayer : public ILayer {
 public:
  GuiLayer() noexcept;
  virtual ~GuiLayer() = default;
  MOVABLE_ONLY(GuiLayer);

 protected:
  Result<void, Error::Layer> onAttached() noexcept override;
  Result<void, Error::Layer> onDetached() noexcept override;
  void onUpdate() noexcept override;
};
}  // namespace pd