#pragma once

#include "pd/backend/backend.hpp"

namespace pd {
// TODO(ryu613): want to integrate my soft renderer but not now
class BackendNoop : public IBackend {
 public:
  BackendNoop() noexcept = default;
  ~BackendNoop() = default;
  MOVABLE_ONLY(BackendNoop);

  [[nodiscard]] GraphicsApi graphicsApi() const noexcept override {
    return IBackend::GraphicsApi::Noop;
  }
};
}  // namespace pd