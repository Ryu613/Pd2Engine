#include "pd/resource/resource_manager.hpp"

namespace pd {
ResourceManager::ResourceManager(IBackend* pBackend) noexcept
    : mBackend(pBackend) {}

void ResourceManager::clearAll() noexcept {
  mTextures.clear();
  mMeshes.clear();
  mRegistry.clear();
}

void ResourceManager::loadAll() noexcept {
  // 1. 加载mesh
  loadResources<MeshResource, MeshResource_t>();
  // 2. 加载texture
}
}  // namespace pd
