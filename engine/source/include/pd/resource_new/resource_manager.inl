#pragma once
#include "pd/resource_new/resource_manager.hpp"
#include "pd/asset/asset.hpp"

namespace pd {
template <typename TAsset, typename Tag>
inline Result<ResourceHandle<Tag>> ResourceManager::registerResourceFromAsset(TAsset* asset) noexcept {}

template <>
Result<ResourceHandle<PrefabResource_t>> ResourceManager::registerResourceFromAsset(GltfAsset* asset) noexcept;
}  // namespace pd