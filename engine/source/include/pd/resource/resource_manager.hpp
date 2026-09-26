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

  /**
   * 用asset信息判重，规则:
   * 1. 若为单个资产，用路径区分(asset里的path)
   * 2. 若为复合资产: prefab resource用asset的path,
   * 包含的各种resource用asset的path加这个asset里面的子数据里的name进行拼接，用冒号隔开,如assets/gltf/a.gltf:mesh_0_0表示a.gltf里的mesh0里的submesh0
   *
   * 例如: asset path: assets/test/1.gltf包含1个mesh, 这个mesh有2个submesh
   * 会分解为1个prefab resource, 注册表的key即为path， 对应的2个mesh resource,会拼接path + ":" +
   * submesh的name(为mesh_0_0,和mesh_0_1),
   *
   * 例如: asset path: assets/shader/a.slang
   * 会生成一个shader resource, key为path
   */
  util::RobinMap<std::string, ResourceEntry, util::StringHasher> mRegistry;

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

  Result<std::unique_ptr<PrefabResource>> createGltfResource(ResourceIdType newId, const std::string& resourceName,
                                                             GltfAsset* gltfAsset) noexcept;
  Result<std::unique_ptr<ShaderResource>> createShaderResource(ResourceIdType newId, const std::string& resourceName,
                                                               ShaderAsset* shaderAsset) noexcept;

  template <typename Tag>
  Result<Handle<StoredTag<Tag>>> registerResource(std::unique_ptr<StoredResource<Tag>> resource) noexcept;

  template <typename StoreTagT>
  Handle<StoreTagT> insertRegistry(const std::string& regKey, ResourceIdType resourceId) noexcept;
};

template <typename Tag>
inline Result<ResourceHandle<ResourceManager::StoredTag<Tag>>> ResourceManager::registerAsset(Asset* asset) noexcept {
  PD_ASSERT_MSG(asset, "asset pointer is null!");
  using ReturnTag = ResourceManager::StoredTag<Tag>;
  // 1. 判重, 若有，返回handle
  // 复合资产和单个资产都用path保证唯一
  const auto resourceName = asset->info().path;
  auto regIt = mRegistry.find(resourceName);
  if (regIt != mRegistry.end()) {
    LOG_INFO("asset duplicate! asset id:{}", asset->id());
    Handle<ReturnTag> duplicateHandle{.data = {
                                          .id = regIt->second.handle.id,
                                          .gen = regIt->second.handle.gen,
                                      }};
    return duplicateHandle;
  }
  // 2. 分类型处理
  // FIXME: not good implementation
  auto newId = nextId<StoredTag<Tag>>();
  switch (asset->info().parseType) {
    using enum AssetType;
    case Gltf: {
      auto res = createGltfResource(newId, resourceName, static_cast<GltfAsset*>(asset));
      if (!res) {
        return make_error<ResourceHandle<ReturnTag>>(res.error().code);
      }
      // gltf resource 对应prefab resource
      saveResource<GltfResource_t>(newId, std::move(res.value()));
      break;
    }
    case Shader: {
      auto res = createShaderResource(newId, resourceName, static_cast<ShaderAsset*>(asset));
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
  auto newHandle = insertRegistry<StoredTag<Tag>>(resourceName, newId);

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
  auto it = mRegistry.find(resource->name());
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
  auto it = mRegistry.find(resource->name());
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

template <typename Tag>
inline Result<ResourceHandle<ResourceManager::StoredTag<Tag>>> ResourceManager::registerResource(
    std::unique_ptr<StoredResource<Tag>> resource) noexcept {
  using ReturnTag = StoredTag<Tag>;
  auto resourceId = resource->id();
  auto handle = insertRegistry<ReturnTag>(resource->name(), resource->id());
  assert(handle);
  saveResource<Tag>(resourceId, std::move(resource));
  return handle;
}

template <typename StoreTagT>
inline ResourceManager::Handle<StoreTagT> ResourceManager::insertRegistry(const std::string& regKey,
                                                                          ResourceIdType resourceId) noexcept {
  LOG_INFO("registering resource: regKey={}, resourceId={}", regKey, resourceId);
  LOG_DEBUG("current registry info:\n size={}", mRegistry.size());
  auto [regInsIt, regSuccess] = mRegistry.emplace(regKey, ResourceEntry{
                                                              .handle =
                                                                  {
                                                                      .id = resourceId,
                                                                      .gen = 0,
                                                                  },
                                                              .refCount = 0,
                                                          });
  PD_ASSERT_MSG(regSuccess, "resource register failed!");

  Handle<StoreTagT> newHandle{.data = {
                                  .id = resourceId,
                                  .gen = 0,
                              }};
  return newHandle;
}
}  // namespace pd