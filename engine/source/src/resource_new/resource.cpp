#include "pd/resource_new/resource.hpp"

namespace pd {
Resource::~Resource() {}

Resource::Resource(AssetIdType assetId, ResourceVariantIdType variantId, const std::string& name)
    : mAssetId(assetId),
      mVariantId(variantId),
      mName(name),
      mStatus(ResourceStatus::Unload) {}

Result<void> Resource::load(Backend& backend) noexcept {
  if (mStatus != ResourceStatus::Unload) {
    LOG_DEBUG("resource status is not [unload]: name={}", mName);
    return {};
  }
  mStatus = ResourceStatus::Loading;
  if (auto res = doLoad(backend); !res) {
    return res;
  }
  mStatus = ResourceStatus::Loaded;
  return {};
}

Result<void> Resource::unload(Backend& backend) noexcept {
  LOG_DEBUG("unloading resource: {}", mName);
  if (mStatus != ResourceStatus::Loaded) {
    return {};
  }
  if (auto res = doUnload(backend); !res) {
    return res;
  }
  mStatus = ResourceStatus::Unload;

  return {};
}
}  // namespace pd