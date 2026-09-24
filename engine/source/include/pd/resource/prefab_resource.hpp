#pragma once

#include "pd/resource/resource.hpp"
#include "pd/backend/backend.hpp"

namespace pd {
class PrefabResource : public Resource {
 public:
  ~PrefabResource() override;

  std::span<const MeshHandle> meshes() const noexcept { return mMeshHandles; }

 protected:
  Result<void> doLoad() noexcept override;
  Result<void> doUnload() noexcept override;

 private:
  friend class ResourceManager;

  std::vector<MeshHandle> mMeshHandles;

  Backend* mBackend = nullptr;

  explicit PrefabResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend);
};
}  // namespace pd