#pragma once

#include "pd/core/utils/map.hpp"
#include "pd/core/utils/pool.hpp"

#include "pd/backend/backend.hpp"

#include "pd/resource_new/resource_ref.hpp"
#include "pd/resource_new/resource/buffer_resource.hpp"
#include "pd/resource_new/resource/texture_resource.hpp"
#include "pd/resource_new/resource/mesh_resource.hpp"
#include "pd/resource_new/resource/prefab_resource.hpp"


namespace pd {
class ResourceManager {
 public:
  ResourceManager(Backend& backend);
  ~ResourceManager();
  DELETE_COPY_MOVE(ResourceManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  template <typename TAsset>
  struct ResourceCreateInfo {
    const TAsset* pAsset = nullptr;
    ResourceVariantIdType variantId = defaultVariantId;
  };

  // 注册时解包，资产转转为子资产，再存储资源，注册资源
  // createInfo相同时，重复调用不会增加引用计数
  template <typename TAsset, typename Tag>
  Result<ResourceRef<Tag>> registerResourceFromAsset(ResourceCreateInfo<TAsset> createInfo) noexcept;

  // handle换强引用
  template <typename Tag>
  Result<ResourceRef<Tag>> acquire(ResourceHandle<Tag> ref) noexcept;

 private:
  template <typename>
  friend class ResourceRef;

  Backend& mBackend;

  struct ResourceKey {
    AssetIdType assetId;  // u32
    // 变体id标识，用于支持同一asset不同resource，可由资源创建者提供
    ResourceVariantIdType variantId = defaultVariantId;  // u32

    friend bool operator==(const ResourceKey&, const ResourceKey&) = default;
  };

  struct ResourceKeyHasher {
    size_t operator()(const ResourceKey& k) const noexcept { return rapidhash(&k, sizeof(k)); }
  };

  template <typename Tag>
  struct ResourceEntry {
    ResourceHandle<Tag> handle;
    u32 refCount = 0;
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

  // 只负责存储，注册
  template <DerivedResource TResource, typename Tag>
  Result<ResourceHandle<Tag>> saveResource(std::unique_ptr<TResource> resource) noexcept;

  template <typename Tag>
  typename TagInfo<Tag>::ResourceType* get(ResourceHandle<Tag> handle) noexcept;
  template <typename Tag>
  void retain(ResourceHandle<Tag> handle) noexcept;
  template <typename Tag>
  void release(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  Result<void> load(ResourceHandle<Tag> handle) noexcept;
  template <typename Tag>
  void unload(ResourceHandle<Tag> handle) noexcept;

  template <typename Tag>
  auto& findPool() const {
    if constexpr (std::is_same_v<Tag, TextureResource_t>) {
      return mTextures;
    } else if constexpr (std::is_same_v<Tag, BufferResource_t>) {
      return mBuffers;
    } else if constexpr (std::is_same_v<Tag, MeshResource_t>) {
      return mMeshes;
    } else if constexpr (std::is_same_v<Tag, PrefabResource_t>) {
      return mPrefabs;
    } else {
      static_assert(false, "resource type not supported!");
    }
  }

  template <typename Tag>
  auto& findRegistry() const {
    if constexpr (std::is_same_v<Tag, TextureResource_t>) {
      return mTextureRegistry;
    } else if constexpr (std::is_same_v<Tag, BufferResource_t>) {
      return mBufferRegistry;
    } else if constexpr (std::is_same_v<Tag, MeshResource_t>) {
      return mMeshRegistry;
    } else if constexpr (std::is_same_v<Tag, PrefabResource_t>) {
      return mPrefabRegistry;
    } else {
      static_assert(false, "resource type not supported!");
    }
  }

  template <typename Tag>
  [[nodiscard]]
  std::optional<ResourceHandle<Tag>> findRegisteredResource(const ResourceKey& key) const noexcept {
    auto& registry = findRegistry<Tag>();
    const auto it = registry.find(key);
    if (it != registry.end()) {
      return it->second.handle;
    }
    LOG_INFO("asset duplicate! asset id:{}", key.assetId);
    return std::nullopt;
  }
};
}  // namespace pd

// resource ref
#include "pd/resource_new/resource_manager.inl"
#include "pd/resource_new/resource_ref.inl"