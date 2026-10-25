#pragma once
#include "pd/resource_new/resource_manager.hpp"
#include "pd/asset/asset.hpp"

namespace pd {
// general
template <typename TAsset, typename Tag>
inline Result<ResourceRef<Tag>> ResourceManager::registerResourceFromAsset(
    ResourceCreateInfo<TAsset> createInfo) noexcept {
  static_assert(false && "not supported asset or resource");
}

template <DerivedResource TResource, typename Tag>
inline Result<ResourceHandle<Tag>> ResourceManager::saveResource(std::unique_ptr<TResource> resource) noexcept {
  auto& pool = findPool<Tag>();
  auto handle = pool.emplace(*(resource.release()));
  PD_ASSERT_MSG(handle, "save resource failed!");
  return handle;
}

// gltf -> prefab
template <>
Result<ResourceRef<PrefabResource_t>> ResourceManager::registerResourceFromAsset(
    ResourceCreateInfo<GltfAsset> createInfo) noexcept;
}  // namespace pd