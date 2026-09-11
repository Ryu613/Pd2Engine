#include "pd/resource/resource.hpp"

#include "pd/backend/backend.hpp"

namespace pd {
Result<void> Resource::load(Backend& backend) noexcept {
  if (mStatus != ResourceStatus::Unload) {
    return {};
  }
  LOG_DEBUG("loading resource: {}", mId);
  mStatus = ResourceStatus::Loading;
  if (auto res = doLoad(backend); !res) {
    return res;
  }
  mStatus = ResourceStatus::Loaded;
  return {};
}

Result<void> Resource::unload(Backend& backend) noexcept {
  LOG_DEBUG("unloading resource: {}", mId);
  if (mStatus != ResourceStatus::Loaded) {
    return {};
  }
  if (auto res = doUnload(backend); !res) {
    return res;
  }
  mStatus = ResourceStatus::Unload;
}
}  // namespace pd