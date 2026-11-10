#pragma once

#include "pd/resource/resource.hpp"
#include "pd/resource/resource_ref.hpp"
#include <span>

namespace pd {
class PrefabResource : public Resource {
 public:
  ~PrefabResource() = default;
  DELETE_COPY(PrefabResource);
  DEFAULT_MOVABLE(PrefabResource);

  std::span<ResourceRef<MeshResource_t>> meshes() noexcept { return mMeshes; }

 protected:
  Result<void> doLoad(LoadContext& ctx) noexcept override;
  Result<void> doUnload(LoadContext& ctx) noexcept override;

 private:
  friend class ResourceManager;

  std::vector<ResourceRef<MeshResource_t>> mMeshes;
  std::vector<ResourceRef<TextureResource_t>> mTextures;

  explicit PrefabResource(u64 sourceId, ResourceVariantIdType variantId = defaultVariantId, std::string name = "prefab")
      : Resource(sourceId, variantId, std::move(name)) {}

  // 注意: 右值不加引用,左值的值类型会加
  void addMesh(ResourceRef<MeshResource_t> ref) noexcept { mMeshes.push_back(std::move(ref)); }
  void addTextures(ResourceRef<TextureResource_t> ref) noexcept { mTextures.push_back(std::move(ref)); }
};

template <>
struct TagInfo<PrefabResource_t> {
  using ResourceType = PrefabResource;
};
}  // namespace pd