#pragma once

#include "pd/resource_new/resource.hpp"
#include "pd/resource_new/resource_ref.hpp"

namespace pd {
class PrefabResource : public Resource {
 public:
  PrefabResource(AssetIdType assetId, ResourceVariantIdType variantId, std::string name)
      : Resource(assetId, variantId, std::move(name)) {}
  ~PrefabResource() = default;
  DELETE_COPY(PrefabResource);
  DEFAULT_MOVABLE(PrefabResource);

  void addMesh(ResourceRef<MeshResource_t> ref) noexcept { mMeshes.push_back(std::move(ref)); }
  void addTextures(ResourceRef<TextureResource_t> ref) noexcept { mTextures.push_back(std::move(ref)); }

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  std::vector<ResourceRef<MeshResource_t>> mMeshes;
  std::vector<ResourceRef<TextureResource_t>> mTextures;
};

template <>
struct TagInfo<PrefabResource_t> {
  using ResourceType = PrefabResource;
};
}  // namespace pd