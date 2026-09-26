#include "pd/resource/mesh_resource.hpp"

namespace pd {
MeshResource::MeshResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend)
    : Resource(id, assetId, name),
      mBackend(&backend) {}

MeshResource::~MeshResource() {}

Result<void> MeshResource::doLoad() noexcept { return {}; }
Result<void> MeshResource::doUnload() noexcept { return {}; }
}  // namespace pd