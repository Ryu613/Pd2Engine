#pragma once

#include "pd/core/utils/map.hpp"
#include "pd/core/utils/pool.hpp"

#include "pd/backend/backend.hpp"

#include "pd/resource/resource_ref.hpp"
#include "pd/resource/resource/buffer_resource.hpp"
#include "pd/resource/resource/texture_resource.hpp"
#include "pd/resource/resource/mesh_resource.hpp"
#include "pd/resource/resource/prefab_resource.hpp"

namespace pd {
template <typename TAsset>
struct ResourceCreateInfo {
  const TAsset* pAsset = nullptr;
  ResourceVariantIdType variantId = defaultVariantId;
};
/**
* @brief 运行时资源管理器，负责CPU/GPU资源的生命周期，加载，引用，句柄等逻辑，是引擎的资源中枢，除了材质相关的资源以外都在此处处理
*/
class ResourceManager {
 public:
  ResourceManager(Backend& backend);
  ~ResourceManager();
  DELETE_COPY_MOVE(ResourceManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  // 注册时解包，资产转转为子资产，再存储资源，注册资源
  // createInfo相同时，重复调用不会增加引用计数
  template <typename TAsset, typename Tag>
  Result<ResourceRef<Tag>> registerResourceFromAsset(ResourceCreateInfo<TAsset> createInfo) noexcept;

  // handle换强引用
  template <typename Tag>
  Result<ResourceRef<Tag>> acquire(ResourceHandle<Tag> ref) noexcept;

  Result<void> gc() noexcept;

 private:
  template <typename>
  friend class ResourceRef;

  Backend& mBackend;

  struct ResourceKey {
    // resource唯一键
    u64 keyId = invalidResourceId;  // u64
    // 变体id标识，用于支持同一asset不同resource，可由资源创建者提供
    ResourceVariantIdType variantId = defaultVariantId;  // u32

    friend bool operator==(const ResourceKey&, const ResourceKey&) = default;

    static ResourceKey fromAsset(AssetIdType assetId, u32 variantId = defaultVariantId) noexcept {
      return {assetId, variantId};
    }

    static ResourceKey fromRuntime(ResourceIdType id, u32 variantId = defaultVariantId) noexcept {
      return {id, variantId};
    }
  };

  struct ResourceKeyHasher {
    size_t operator()(const ResourceKey& k) const noexcept { return rapidhash(&k, sizeof(k)); }
  };

  template <typename Tag>
  struct ResourceEntry {
    ResourceHandle<Tag> handle;
    u32 refCount = 0;
    std::array<ResourceKey> children;
  };

  template <typename Tag>
  using Registry = util::RobinMap<ResourceKey, ResourceEntry<Tag>, ResourceKeyHasher>;

  // basic resource
  Registry<BufferResource_t> mBufferRegistry;
  Pool<BufferResource, BufferResource_t> mBuffers{64};
  Registry<TextureResource_t> mTextureRegistry;
  Pool<TextureResource, TextureResource_t> mTextures{64};

  // prefabs
  Registry<MeshResource_t> mMeshRegistry;
  Pool<MeshResource, MeshResource_t> mMeshes{64};
  // gltf, etc
  Registry<PrefabResource_t> mPrefabRegistry;
  Pool<PrefabResource, PrefabResource_t> mPrefabs{64};

  // 只负责存储，不负责判重，计数，注册
  template <DerivedResource TResource, typename Tag>
  Result<ResourceHandle<Tag>> saveResource(std::unique_ptr<TResource> resource) noexcept;

  // 判重，注册
  template <typename Tag>
  Result<ResourceHandle<Tag>> saveRegistry(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  typename TagInfo<Tag>::ResourceType* get(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  Result<void> retain(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  Result<void> release(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  Result<void> load(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  void unload(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  auto& findPool() noexcept;

  template <typename Tag>
  auto& findRegistry() noexcept;

  template <typename Tag>
  ResourceEntry<Tag>* findEntry(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  ResourceKey getResourceKey(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  [[nodiscard]]
  std::optional<ResourceHandle<Tag>> findRegisteredResource(const ResourceKey& key) noexcept;
};
}  // namespace pd

// resource ref
#include "pd/resource/resource_manager.inl"
#include "pd/resource/resource_ref.inl"