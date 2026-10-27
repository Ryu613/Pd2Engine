#pragma once

#include "pd/resource/resource.hpp"
#include "pd/resource/resource_ref.hpp"

namespace pd {
class PrefabResource : public Resource {
 public:
  ~PrefabResource() = default;
  DELETE_COPY(PrefabResource);
  DEFAULT_MOVABLE(PrefabResource);

  // 注意: 右值不加引用,左值的值类型会加
  void addMesh(ResourceRef<MeshResource_t> ref) noexcept { mMeshes.push_back(std::move(ref)); }
  void addTextures(ResourceRef<TextureResource_t> ref) noexcept { mTextures.push_back(std::move(ref)); }

 protected:
  Result<void> doLoad(Backend& backend) noexcept override;
  Result<void> doUnload(Backend& backend) noexcept override;

 private:
  friend class ResourceManager;

  std::vector<ResourceRef<MeshResource_t>> mMeshes;
  std::vector<ResourceRef<TextureResource_t>> mTextures;

  explicit PrefabResource(AssetIdType assetId, ResourceVariantIdType variantId = defaultVariantId,
                          std::string name = "prefab")
      : Resource(assetId, variantId, std::move(name)) {}
};

template <>
struct TagInfo<PrefabResource_t> {
  using ResourceType = PrefabResource;
};
}  // namespace pd