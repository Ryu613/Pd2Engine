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

  template <typename TAsset, typename Tag>
  Result<ResourceHandle<Tag>> registerResource(TAsset* asset) noexcept;

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

  struct ResourceKey {};

  template <typename Tag>
  using Registry = util::RobinMap<ResourceKey, ResourceHandle<Tag>>;
  Registry<PrefabResource_t> mRegistry;

  Pool<PrefabResource, PrefabResource_t> mPrefabs;

  template <typename Tag>
  typename TagInfo<Tag>::ResourceType* get(ResourceHandle<Tag> handle) const noexcept;
  template <typename Tag>
  void retain(ResourceHandle<Tag> handle) noexcept;
  template <typename Tag>
  void release(ResourceHandle<Tag> handle) noexcept;
};
}  // namespace pd

// resource ref
#include "pd/resource_new/resource_manager.inl"
#include "pd/resource_new/resource_ref.inl"