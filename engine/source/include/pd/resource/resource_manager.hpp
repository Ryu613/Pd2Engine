#pragma once

#include "pd/core/utils/map.hpp"
#include "pd/asset/asset_types.hpp"
#include "pd/asset/asset.hpp"
#include "pd/resource/mesh_resource.hpp"
#include "pd/resource/texture_resource.hpp"
#include "pd/resource/shader_resource.hpp"
#include "pd/resource/prefab_resource.hpp"

namespace pd {
// traits
template <typename Tag>
struct TagInfo;

// 用来支持tag与resource 多对一关系
template <>
struct TagInfo<ShaderResource_t> {
  using Resource = ShaderResource;
  using Tag = ShaderResource_t;
};
template <>
struct TagInfo<TextureResource_t> {
  using Resource = TextureResource;
  using Tag = TextureResource_t;
};

template <>
struct TagInfo<MeshResource_t> {
  using Resource = MeshResource;
  using Tag = MeshResource_t;
};

template <>
struct TagInfo<GltfResource_t> {
  using Resource = PrefabResource;
  using Tag = PrefabResource_t;
};

class Backend;
class ResourceManager {
 public:
  template <typename Tag>
  using Handle = ResourceHandle<Tag>;

  template <typename Tag>
  using StoredResource = typename TagInfo<Tag>::Resource;
  template <typename Tag>
  using StoredTag = typename TagInfo<Tag>::Tag;

  explicit ResourceManager(Backend* backend);
  ~ResourceManager();
  DELETE_COPY_MOVE(ResourceManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  template <typename Tag>
  Result<Handle<StoredTag<Tag>>> registerAsset(Asset* asset) noexcept;

  template <typename Tag>
  Result<void> loadResource(Handle<Tag> handle) noexcept;

  template <typename Tag>
  Result<void> unloadResource(Handle<Tag> handle) noexcept;

  template <typename Tag>
  auto* getResource(Handle<Tag> handle) noexcept;

  // Result<void> clearAll() noexcept;

  // Result<void> loadAll() noexcept;

  // Result<void> gc() noexcept;

 private:
  struct ResourceEntry {
    BaseHandle handle;
    u32 refCount = 0;
    bool isAlive = false;
  };
  Backend* mBackend = nullptr;

  // 用asset id判重
  util::RobinMap<AssetIdType, ResourceEntry> mRegistry;

  template <typename T, typename Tag>
  using Storage = util::RobinMap<ResourceIdType, std::unique_ptr<T>>;
  Storage<MeshResource, MeshResource_t> mMeshes;
  Storage<TextureResource, TextureResource_t> mTextures;
  Storage<ShaderResource, ShaderResource_t> mShaders;
  Storage<PrefabResource, PrefabResource_t> mPrefabs;

  template <typename StoredTagT>
  auto& findStorage() noexcept;

  template <typename Tag>
  void saveResource(ResourceIdType id, std::unique_ptr<StoredResource<Tag>> pResource) noexcept;

  template <typename Tag>
  uint32_t nextId() noexcept;

  Result<std::unique_ptr<PrefabResource>> createGltfResource(ResourceIdType newId, GltfAsset* gltfAsset) noexcept;
  Result<std::unique_ptr<ShaderResource>> createShaderResource(ResourceIdType newId, ShaderAsset* shaderAsset) noexcept;
};

template <typename Tag>
inline Result<ResourceHandle<ResourceManager::StoredTag<Tag>>> ResourceManager::registerAsset(Asset* asset) noexcept {
  PD_ASSERT_MSG(asset, "asset pointer is null!");
  using ReturnTag = ResourceManager::StoredTag<Tag>;
  // 1. 判重
  auto regIt = mRegistry.find(asset->id());
  if (regIt != mRegistry.end()) {
    LOG_INFO("asset duplicate! asset id:{}", asset->id());
    return {};
  }
  // 2. 分类型处理
  // FIXME: not good implementation
  auto newId = nextId<Tag>();
  switch (asset->info().parseType) {
    using enum AssetType;
    case Gltf: {
      auto res = createGltfResource(newId, static_cast<GltfAsset*>(asset));
      if (!res) {
        return make_error<ResourceHandle<ReturnTag>>(res.error().code);
      }
      // gltf resource 对应prefab resource
      saveResource<GltfResource_t>(newId, std::move(res.value()));
      break;
    }
    case Shader: {
      auto res = createShaderResource(newId, static_cast<ShaderAsset*>(asset));
      if (!res) {
        return make_error<ResourceHandle<ReturnTag>>(res.error().code);
      }
      saveResource<ShaderResource_t>(newId, std::move(res.value()));
      break;
    }
    default:
      return make_error<ResourceHandle<ReturnTag>>(ErrorCode::ResourceTypeNotSupported);
  }
  // 3. 更新注册表
  auto [regInsIt, regSuccess] = mRegistry.emplace(asset->id(), ResourceEntry{
                                                                   .handle =
                                                                       {
                                                                           .id = newId,
                                                                           .gen = 0,
                                                                       },
                                                                   .refCount = 0,
                                                               });
  PD_ASSERT_MSG(regSuccess, "shader resource register failed!");

  Handle<ReturnTag> newHandle{.data = {
                                  .id = newId,
                              }};

  return newHandle;
}

template <typename StoredTagT>
inline auto& ResourceManager::findStorage() noexcept {
  if constexpr (std::is_same_v<StoredTagT, TextureResource_t>) {
    return mTextures;
  } else if constexpr (std::is_same_v<StoredTagT, MeshResource_t>) {
    return mMeshes;
  } else if constexpr (std::is_same_v<StoredTagT, ShaderResource_t>) {
    return mShaders;
  } else if constexpr (std::is_same_v<StoredTagT, PrefabResource_t>) {
    return mPrefabs;
  } else {
    static_assert(false, "resource type not supported!");
  }
}

template <typename Tag>
inline auto* ResourceManager::getResource(Handle<Tag> handle) noexcept {
  auto& dataPool = findStorage<Tag>();
  auto it = dataPool.find(handle.data.id);
  if (it != dataPool.end()) {
    // todo: gen equality
    return it->second.get();
  }
  using ResourcePtr = decltype(it->second.get());
  return static_cast<ResourcePtr>(nullptr);
}

template <typename Tag>
inline Result<void> ResourceManager::loadResource(Handle<Tag> handle) noexcept {
  auto* resource = getResource(handle);
  if (resource == nullptr) {
    LOG_ERROR("resource id: {} not exist, cannot load!", handle.data.id);
    return make_error<void>(ErrorCode::ResourceLoadFailed);
  }
  if (auto res = resource->load(); !res) {
    return res;
  }
  // 更新注册表引用计数
  auto it = mRegistry.find(resource->assetId());
  PD_ASSERT(it != mRegistry.end());

  it.value().refCount++;
  return {};
}

template <typename Tag>
inline Result<void> ResourceManager::unloadResource(Handle<Tag> handle) noexcept {
  auto* resource = getResource(handle);
  if (resource == nullptr) {
    LOG_ERROR("resource id: {} not exist, cannot unload!", handle.data.id);
    return make_error<void>(ErrorCode::ResourceLoadFailed);
  }
  if (auto res = resource->unload(); !res) {
    return res;
  }

  // 更新注册表引用计数
  auto it = mRegistry.find(resource->assetId());
  PD_ASSERT(it != mRegistry.end());

  it.value().refCount++;
  return {};
}

template <typename Tag>
inline void ResourceManager::saveResource(ResourceIdType id, std::unique_ptr<StoredResource<Tag>> pResource) noexcept {
  auto& storage = findStorage<StoredTag<Tag>>();
  auto [insIt, insSuccess] = storage.emplace(id, std::move(pResource));
  PD_ASSERT_MSG(insSuccess, "resource insert failed!");
}

template <typename Tag>
inline ResourceIdType ResourceManager::nextId() noexcept {
  static ResourceIdType tagId;
  return tagId++;
}
}  // namespace pd