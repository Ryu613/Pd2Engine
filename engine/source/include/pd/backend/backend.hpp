#pragma once

namespace pd {
class IWindow;
class Backend {
 public:
  Backend();
  ~Backend();
  DELETE_COPY_MOVE(Backend);

  Result<void> init(const BackendConfig& config) noexcept;
  Result<void> destroy() noexcept;

  [[nodiscard]] BackendApi backendApi() const noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd