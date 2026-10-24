#pragma once

#include "pd/core/utils/map.hpp"
#include "pd/core/utils/pool.hpp"

#include "pd/backend/backend.hpp"

#include "pd/resource_new/resource_ref.hpp"
#include "pd/resource_new/resource/prefab_resource.hpp"

namespace pd {
class ResourceManager {
 public:
  ResourceManager(Backend& backend);
  ~ResourceManager();
  DELETE_COPY_MOVE(ResourceManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  // 注册时解包，资产转转为子资产，再存储资源，注册资源
  template <typename TAsset, typename Tag>
  Result<ResourceHandle<Tag>> registerResourceFromAsset(TAsset* asset) noexcept;

  // 拿到资源的强引用
  template <typename Tag>
  Result<ResourceRef<Tag>> acquire(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  Result<void> load(ResourceHandle<Tag> handle) noexcept;
  template <typename Tag>
  Result<void> unload(ResourceHandle<Tag> handle) noexcept;

 private:
  template <typename>
  friend class ResourceRef;

  Backend& mBackend;

  struct ResourceKey {
    AssetIdType assetId;
    ResourceIdType resourceId;

    friend bool operator==(const ResourceKey&, const ResourceKey&) = default;
  };

  struct ResourceKeyHasher {
    size_t operator()(const ResourceKey& k) const noexcept { return rapidhash(&k, sizeof(k)); }
  };

  template <typename Tag>
  using Registry = util::RobinMap<ResourceKey, ResourceHandle<Tag>, ResourceKeyHasher>;

  // basic resource
  Registry<BufferResource_t> mBufferRegistry;
  Pool<BufferResource, BufferResource_t> mBuffers{64};
  Registry<MeshResource_t> mTextureRegistry;
  Pool<TextureResource, TextureResource_t> mTextures{64};

  // prefabs
  Registry<MeshResource_t> mMeshRegistry;
  Pool<MeshResource, MeshResource_t> mMeshes{64};
  // gltf, etc
  Registry<PrefabResource_t> mPrefabRegistry;
  Pool<PrefabResource, PrefabResource_t> mPrefabs{64};

  template <typename Tag>
  typename TagInfo<Tag>::ResourceType* get(ResourceHandle<Tag> handle) noexcept;
  template <typename Tag>
  void retain(ResourceHandle<Tag> handle) noexcept;
  template <typename Tag>
  void release(ResourceHandle<Tag> handle) noexcept;
};
}  // namespace pd

// resource ref
#include "pd/resource_new/resource_manager.inl"
#include "pd/resource_new/resource_ref.inl"