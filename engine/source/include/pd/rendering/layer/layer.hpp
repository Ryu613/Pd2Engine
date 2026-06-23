#pragma once

#include "pd/core/error.hpp"

namespace pd {
class ILayer {
 public:
  ILayer() noexcept = default;
  virtual ~ILayer() = default;
  MOVABLE_ONLY(ILayer);

 protected:
  virtual std::string name() const noexcept { return "base layer"; }
  virtual Result<void, Error::Layer> onAttached() noexcept { return {}; }
  virtual Result<void, Error::Layer> onDetached() noexcept { return {}; }
  virtual void onUpdate() noexcept {}

 private:
  friend class Engine;
};

template <typename T>
concept BaseOfLayer = std::derived_from<T, ILayer>;
}  // namespace pd