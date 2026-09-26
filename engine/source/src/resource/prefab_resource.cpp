#include "pd/resource/prefab_resource.hpp"

namespace pd {
PrefabResource::PrefabResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend)
    : Resource(id, assetId, name),
      mBackend(&backend) {}

PrefabResource::~PrefabResource() {}

Result<void> PrefabResource::doLoad() noexcept {
  LOG_INFO("loading prefab resource:id={},name={}", id(), name());
  return {};
}
Result<void> PrefabResource::doUnload() noexcept {
  LOG_INFO("unloading prefab resource:id={},name={}", id(), name());
  return {};
}
}  // namespace pd