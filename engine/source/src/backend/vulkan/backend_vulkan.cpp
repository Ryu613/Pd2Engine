#include "pd/backend/backend.hpp"

namespace pd {
class Backend::Impl {
 public:
  Result<void> init(const BackendConfig& config) noexcept {
    mConfig = config;
    return {};
  }

  Result<void> destroy() noexcept { return {}; }

 private:
  BackendConfig mConfig{};
};

Backend::Backend()
    : mImpl(std::make_unique<Impl>()) {}

Backend::~Backend() = default;

Result<void> Backend::init(const BackendConfig& config) noexcept { return mImpl->init(config); }

Result<void> Backend::destroy() noexcept { return mImpl->destroy(); }
}  // namespace pd