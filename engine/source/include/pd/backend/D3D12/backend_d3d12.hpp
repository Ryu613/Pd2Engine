#pragma once

#include "pd/backend/backend.hpp"

namespace pd {
// TODO(author): interested in D3DX but have no time for now!
class BackendD3D12 : public IBackend {
 public:
  BackendD3D12() noexcept = default;
  ~BackendD3D12() = default;
  MOVABLE_ONLY(BackendD3D12);

  [[nodiscard]] GraphicsApi graphicsApi() const noexcept override {
    return IBackend::GraphicsApi::D3D12;
  }
};
}  // namespace pd