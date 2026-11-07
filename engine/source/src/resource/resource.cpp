#include "pd/resource/resource.hpp"

namespace pd {
Resource::~Resource() {}

Resource::Resource(u64 sourceId, ResourceVariantIdType variantId, std::string name)
    : mSourceId(sourceId),
      mVariantId(variantId),
      mName(std::move(name)),
      mStatus(ResourceStatus::Unload) {}

Result<void> Resource::load(Backend& backend, u32& loadCount) noexcept {
  if (mStatus == ResourceStatus::Unload) {
    mStatus = ResourceStatus::Loading;
    if (auto res = doLoad(backend); !res) {
      mStatus = ResourceStatus::Unload;
      return res;
    }
    mStatus = ResourceStatus::Loaded;
  }
  loadCount++;
  LOG_DEBUG("resource is not loaded because the status is not [unload]: name={}", mName);
  return {};
}

Result<void> Resource::unload(Backend& backend, u32& loadCount) noexcept {
  if (mStatus != ResourceStatus::Loaded) {
    return {};
  }
  if(loadCount > 1) {
    loadCount--;
    return {};
  }
  if (auto res = doUnload(backend); !res) {
    return res;
  }
  mStatus = ResourceStatus::Unload;
  loadCount--;
  return {};
}
}  // namespace pd