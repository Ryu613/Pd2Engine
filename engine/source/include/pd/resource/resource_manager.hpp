#pragma once

#include "pd/core/utils/map.hpp"
#include "pd/asset/asset_types.hpp"
#include "pd/asset/asset.hpp"
#include "pd/resource/mesh_resource.hpp"
#include "pd/resource/texture_resource.hpp"
#include "pd/resource/shader_resource.hpp"
#include "pd/resource/prefab_resource.hpp"

namespace pd {
class Backend;
class ResourceManager {
 public:
  template <typename Tag>
  using Handle = ResourceHandle<Tag>;

  explicit ResourceManager(Backend* backend);
  ~ResourceManager();
  DELETE_COPY_MOVE(ResourceManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  template <typename Tag>
  Result<Handle<Tag>> registerAsset(Asset* asset) noexcept;

  template <typename Tag>
  Result<void> loadResource(Handle<Tag> handle) noexcept;

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

  template <typename Tag>
  auto& findStorage() noexcept;

  template <typename Tag>
  auto* getResource(Handle<Tag> handle) noexcept;

  template <typename Tag>
  void saveResource(ResourceIdType id, std::unique_ptr<Resource> pResource) noexcept;

  template <typename Tag>
  uint32_t nextId() noexcept;

  Result<std::unique_ptr<PrefabResource>> createGltfResource(ResourceIdType newId, GltfAsset* gltfAsset) noexcept;
  Result<std::unique_ptr<ShaderResource>> createShaderResource(ResourceIdType newId, ShaderAsset* shaderAsset) noexcept;
};

template <typename Tag>
inline Result<ResourceHandle<Tag>> ResourceManager::registerAsset(Asset* asset) noexcept {
  PD_ASSERT_MSG(asset, "asset pointer is null!");
  // 1. 判重
  auto regIt = mRegistry.find(asset->id());
  if (regIt != mRegistry.end()) {
    LOG_INFO("asset duplicate! asset id:{}", asset->id());
    return {};
  }
  // 2. 分类型处理
  auto newId = nextId<Tag>();
  switch (asset->info().parseType) {
    using enum AssetType;
    case Gltf: {
      auto res = createGltfResource(newId, static_cast<GltfAsset*>(asset));
      if (!res) {
        return make_error<ResourceHandle<Tag>>(res.error().code);
      }
      saveResource<Tag>(newId, std::move(res.value()));
      break;
    }
    case Shader: {
      auto res = createShaderResource(newId, static_cast<ShaderAsset*>(asset));
      if (!res) {
        return make_error<ResourceHandle<Tag>>(res.error().code);
      }
      saveResource<Tag>(newId, std::move(res.value()));
      break;
    }
    default:
      return make_error<ResourceHandle<Tag>>(ErrorCode::ResourceTypeNotSupported);
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

  Handle<Tag> newHandle{.data = {
                            .id = newId,
                        }};

  return newHandle;
}

template <typename Tag>
inline auto& ResourceManager::findStorage() noexcept {
  if constexpr (std::is_same_v<Tag, TextureResource_t>) {
    return mTextures;
  } else if constexpr (std::is_same_v<Tag, MeshResource_t>) {
    return mMeshes;
  } else if constexpr (std::is_same_v<Tag, ShaderResource_t>) {
    return mShaders;
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
  return {};
}

template <typename Tag>
inline void ResourceManager::saveResource(ResourceIdType id, std::unique_ptr<Resource> resource) noexcept {
  auto& storage = findStorage<Tag>();
  // FIXME
  // auto [insIt, insSuccess] = storage.emplace(id, std::move(resource));
  // PD_ASSERT_MSG(insSuccess, "resource insert failed!");
}

template <typename Tag>
inline ResourceIdType ResourceManager::nextId() noexcept {
  static ResourceIdType tagId;
  return tagId++;
}
}  // namespace pd