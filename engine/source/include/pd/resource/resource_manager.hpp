#pragma once

#include "pd/core/utils/map.hpp"
#include "pd/asset/asset_types.hpp"
#include "pd/resource/mesh_resource.hpp"
#include "pd/resource/texture_resource.hpp"
#include "pd/resource/shader_resource.hpp"

namespace pd {
class Backend;
class Asset;
class GltfAsset;
class ShaderAsset;
class ShaderManager;
class ResourceManager {
 public:
  template <typename Tag>
  using Handle = ResourceHandle<Tag>;

  explicit ResourceManager(Backend* backend);
  ~ResourceManager();
  DELETE_COPY_MOVE(ResourceManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  Result<void> createResourcesFromAsset(Asset* asset) noexcept;

  Result<void> clearAll() noexcept;

  Result<void> loadAll() noexcept;

  Result<void> gc() noexcept;

 private:
  struct ResourceEntry {
    BaseHandle handle;
    u32 refCount = 0;
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
  uint32_t nextId() noexcept;

  Result<void> registerGltfAsset(GltfAsset* gltfAsset) noexcept;
  Result<void> registerShaderAsset(ShaderAsset* shaderAsset) noexcept;
};

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
    return &(it->second.resource);
  }
  using ResourcePtr = decltype(&(it->second.resource));
  return static_cast<ResourcePtr>(nullptr);
}

template <typename Tag>
inline ResourceIdType ResourceManager::nextId() noexcept {
  static ResourceIdType tagId;
  return tagId++;
}
}  // namespace pd