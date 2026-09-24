#include "pd/resource/prefab_resource.hpp"

namespace pd {
PrefabResource::PrefabResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend)
    : Resource(id, assetId, name),
      mBackend(&backend) {}

PrefabResource::~PrefabResource() {}

Result<void> PrefabResource::doLoad() noexcept { return {}; }
Result<void> PrefabResource::doUnload() noexcept { return {}; }
}  // namespace pd