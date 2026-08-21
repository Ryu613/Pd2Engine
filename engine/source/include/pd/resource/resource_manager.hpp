#pragma once

#include "pd/resource/resource_alias.hpp"
#include "pd/core/utils/hash.hpp"
#include "pd/core/utils/pool.hpp"
#include "tsl/robin_map.h"

namespace pd {
class Asset;
class ResourceManager {
 public:
  template <typename T>
  using Handle = ResourceHandle<T>;

  ResourceManager() noexcept = default;
  ~ResourceManager() = default;
  DELETE_COPY_MOVE(ResourceManager);

  void initialize() noexcept;
  void destroy() noexcept;

  Result<void> loadAsset(Asset* asset) noexcept;

 private:
  template <typename Tag>
  struct ResourceEntry {
    Handle<Tag> handle;
    u32 refCount = 0u;
  };
  bool mInitialized = false;
  // todo(ryu613): integrate arena allocator
  struct Hasher {
    size_t operator()(const std::string& key) const noexcept {
      return static_cast<size_t>(rapidhash(key.data(), key.size()));
    }
  };
  template <typename Tag>
  using Registry = tsl::robin_map<Resource::IdType, ResourceEntry<Tag>, Hasher>;
  // key=resource id
  Registry<MeshResource_t> mMeshRegistry;
  Registry<TextureResource_t> mTextureRegistry;
  Pool<MeshResource, MeshResource_t> mMeshData{1024};
  Pool<TextureResource, TextureResource_t> mTextureData{1024};

  u64 generateResourceId(std::string_view str) noexcept;
};
}  // namespace pd