#pragma once
#include "pd/resource_new/resource_manager.hpp"
#include "pd/asset/asset.hpp"

namespace pd {
template <typename TAsset, typename Tag>
inline Result<ResourceHandle<Tag>> ResourceManager::registerResource(TAsset* asset) noexcept {}

template <>
Result<ResourceHandle<PrefabResource_t>> ResourceManager::registerResource(GltfAsset* asset) noexcept;
}  // namespace pd