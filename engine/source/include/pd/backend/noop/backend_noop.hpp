#pragma once

#include "pd/backend/backend.hpp"

namespace pd {
// TODO(author): interested in D3DX but have no time for now!
class BackendNoop : public IBackend {
 public:
  BackendNoop() noexcept = default;
  ~BackendNoop() = default;
  MOVABLE_ONLY(BackendNoop);

  [[nodiscard]] GraphicsApi graphicsApi() const noexcept override {
    return IBackend::GraphicsApi::NOOP;
  }
};
}  // namespace pd