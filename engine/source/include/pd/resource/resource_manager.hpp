#pragma once

#include "pd/resource/resource_alias.hpp"
#include "pd/core/utils/map.hpp"
#include "pd/core/utils/pool.hpp"
#include "tsl/robin_map.h"

namespace pd {
class Asset;
class IBackend;
class ResourceManager {
 public:
  template <typename T>
  using Handle = ResourceHandle<T>;

  ResourceManager() noexcept = default;
  ~ResourceManager() = default;
  DELETE_COPY_MOVE(ResourceManager);

  void initialize(IBackend* pBackend) noexcept;
  void destroy() noexcept;

  Result<void> loadAsset(Asset* asset) noexcept;

  void clearAll() noexcept;

  void loadAll() noexcept;

  void gc() noexcept;

 private:
  template <typename Tag>
  struct ResourceEntry {
    Handle<Tag> handle;
    u32 refCount = 0u;
  };
  IBackend* mBackend = nullptr;
  bool mInitialized = false;
  // todo(ryu613): integrate arena allocator
  template <typename Tag>
  using Registry = util::RobinMap<Resource::IdType, ResourceEntry<Tag>, util::StringHasher>;
  // key=resource id
  Registry<MeshResource_t> mMeshRegistry;
  Registry<TextureResource_t> mTextureRegistry;
  Pool<MeshResource, MeshResource_t> mMeshData{1024};
  Pool<TextureResource, TextureResource_t> mTextureData{1024};

  template <typename Tag>
  auto& findPool() {
    if constexpr (std::is_same_v<Tag, TextureResource_t>) {
      return mTextureData;
    } else if constexpr (std::is_same_v<Tag, MeshResource_t>) {
      return mMeshData;
    } else {
      static_assert(false, "resource type not supported!");
    }
  }

  template <typename Tag>
  auto& findRegistry() {
    if constexpr (std::is_same_v<Tag, TextureResource_t>) {
      return mTextureRegistry;
    } else if constexpr (std::is_same_v<Tag, MeshResource_t>) {
      return mMeshRegistry;
    } else {
      static_assert(false, "resource type not supported!");
    }
  }

  template <typename Tag>
  void loadResources() noexcept {
    auto& registry = findRegistry<Tag>();
    auto& pool = findPool<Tag>();
    for (auto& [id, entry] : registry) {
      if (entry.refCount == 0) {
        continue;
      }
      auto* pResource = pool.get(entry.handle);
      if (pResource == nullptr) {
        continue;
      }
      pResource->load(*mBackend);
    }
  }

  template <typename Tag>
  void unloadResources(bool isAllClear = false) noexcept {
    auto& registry = findRegistry<Tag>();
    auto& pool = findPool<Tag>();
    for (auto& [id, entry] : registry) {
      if (entry.refCount == 0 || isAllClear) {
        auto* pResource = pool.get(entry.handle);
        if (pResource == nullptr) {
          continue;
        }
        pResource->unload(*mBackend);
      }
    }
  }
};
}  // namespace pd