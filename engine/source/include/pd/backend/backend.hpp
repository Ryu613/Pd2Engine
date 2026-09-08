#pragma once

#include "pd/backend/cmd_recorder.hpp"

namespace pd {

struct FrameData {
  uint32_t swapchainImageIndex = u32_max;
  CommandRecorder cmdRecorder;
};

class Backend {
 public:
  Backend();
  ~Backend();
  DELETE_COPY_MOVE(Backend);

  Result<void> init(const BackendConfig& config) noexcept;
  Result<void> destroy() noexcept;

  [[nodiscard]] BackendApi backendApi() const noexcept;

  FrameData beginFrame() noexcept;

  void endFrame(const FrameData& frameData) noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd