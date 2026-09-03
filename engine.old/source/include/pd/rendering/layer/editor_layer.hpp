#pragma once

#include "pd/rendering/layer/layer.hpp"

namespace pd {
class EditorLayer : public ILayer {
 public:
  EditorLayer() noexcept;
  virtual ~EditorLayer() = default;
  MOVABLE_ONLY(EditorLayer);

 protected:
  Result<void> onAttached() noexcept override;
  Result<void> onDetached() noexcept override;
  void onUpdate() noexcept override;
};
}  // namespace pd