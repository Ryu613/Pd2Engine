#pragma once

#include "pd/core/utils/map.hpp"
#include "pd/resource/mesh_resource.hpp"
#include "pd/resource/texture_resource.hpp"

namespace pd {
class Backend;
class Asset;
class ResourceManager {
 public:
  using IdType = Resource::IdType;
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

  util::RobinMap<std::string, ResourceEntry, util::StringHasher> mRegistry;

  template <typename T>
  using Data = util::RobinMap<IdType, std::unique_ptr<T>>;
  Data<MeshResource> mMeshes;
  Data<TextureResource> mTextures;

  template <typename Tag>
  auto& findData() noexcept;

  template <typename T, typename Tag>
  T* getResource(Handle<Tag> handle) noexcept;
};

template <typename Tag>
inline auto& ResourceManager::findData() noexcept {
  if constexpr (std::is_same_v<Tag, TextureResource_t>) {
    return mTextures;
  } else if constexpr (std::is_same_v<Tag, MeshResource_t>) {
    return mMeshes;
  } else {
    static_assert(false, "resource type not supported!");
  }
}

template <typename T, typename Tag>
inline T* ResourceManager::getResource(Handle<Tag> handle) noexcept {
  return nullptr;
}
}  // namespace pd