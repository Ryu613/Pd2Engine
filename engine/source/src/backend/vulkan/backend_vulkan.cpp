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
  BackendConfig mConfig;
};

Backend::Backend() = default;
Backend::~Backend() = default;

Result<void> Backend::init(const BackendConfig& config) noexcept {
  if (auto res = mImpl->init(config); !res) {
    return res;
  }
  return {};
}
Result<void> Backend::destroy() noexcept {
  if (auto res = mImpl->destroy(); !res) {
    return res;
  }
  return {};
}
}  // namespace pd