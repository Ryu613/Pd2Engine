#include "pd/resource/resource.hpp"

namespace pd {

Resource::~Resource() {}

Resource::Resource(ResourceIdType id, const std::string& name)
    : mId(id),
      mName(name) {}

Result<void> Resource::load() noexcept {
  if (mStatus != ResourceStatus::Unload) {
    LOG_DEBUG("resource status is not [unload]: id={}, name={}", mId, mName);
    return {};
  }
  mStatus = ResourceStatus::Loading;
  if (auto res = doLoad(); !res) {
    return res;
  }
  mStatus = ResourceStatus::Loaded;
  return {};
}

Result<void> Resource::unload() noexcept {
  LOG_DEBUG("unloading resource: {}", mId);
  if (mStatus != ResourceStatus::Loaded) {
    return {};
  }
  if (auto res = doUnload(); !res) {
    return res;
  }
  mStatus = ResourceStatus::Unload;

  return {};
}
}  // namespace pd