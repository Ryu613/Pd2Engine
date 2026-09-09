#pragma once

#include "pd/backend/command_recorder.hpp"
#include "pd/backend/backend_types.hpp"

namespace pd {

class Backend {
 public:
  Backend();
  ~Backend();
  DELETE_COPY_MOVE(Backend);

  Result<void> init(const BackendConfig& config) noexcept;
  Result<void> destroy() noexcept;

  [[nodiscard]] BackendApi backendApi() const noexcept;

  FrameData beginFrame() noexcept;

  void endFrame(const CommandRecorder& cmdRecorder) noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd