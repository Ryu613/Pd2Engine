#include "pd/resource/prefab_resource.hpp"

namespace pd {
PrefabResource::PrefabResource(ResourceIdType id, const std::string& name, Backend& backend)
    : Resource(id, name),
      mBackend(&backend) {}

PrefabResource::~PrefabResource() {}

Result<void> PrefabResource::doLoad() noexcept { return {}; }
Result<void> PrefabResource::doUnload() noexcept { return {}; }
}  // namespace pd