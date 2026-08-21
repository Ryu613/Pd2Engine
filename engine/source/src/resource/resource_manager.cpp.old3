#include "pd/resource/resource_manager.hpp"

namespace pd {
ResourceManager::~ResourceManager() { destroy(); }

void ResourceManager::clearAll() noexcept {
  unloadResources<MeshResource, MeshResource_t>(true);
  unloadResources<TextureResource, TextureResource_t>(true);
  mTextures.clear();
  mMeshes.clear();
  mRegistry.clear();
}

void ResourceManager::loadAll() noexcept {
  // 1. 加载mesh
  loadResources<MeshResource, MeshResource_t>();
  // 2. 加载texture
  //   loadResources<TextureResource, TextureResource_t>();
}

void ResourceManager::initialize(IBackend* pBackend) noexcept {
  if (mInitialized) {
    return;
  }
  mBackend = pBackend;

  // TODO(ryu613): 加载默认资源

  mInitialized = true;
}

void ResourceManager::destroy() noexcept {
  if (!mInitialized) {
    return;
  }
  clearAll();
}
}  // namespace pd
