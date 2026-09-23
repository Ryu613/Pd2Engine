#include "pd/resource/resource_manager.hpp"

#include "pd/asset/asset.hpp"

namespace pd {
ResourceManager::ResourceManager(Backend* backend)
    : mBackend(backend) {}

ResourceManager::~ResourceManager() {}

Result<void> ResourceManager::init() noexcept { return {}; }

Result<void> ResourceManager::destroy() noexcept { return {}; }

Result<std::unique_ptr<PrefabResource>> ResourceManager::createGltfResource(ResourceIdType newId,
                                                                            GltfAsset* gltfAsset) noexcept {
  LOG_INFO("create gltf resource {}", gltfAsset->info().name);
  std::unique_ptr<PrefabResource> prefabResource;
  return prefabResource;
}
Result<std::unique_ptr<ShaderResource>> ResourceManager::createShaderResource(ResourceIdType newId,
                                                                              ShaderAsset* shaderAsset) noexcept {
  auto shaderRsc = std::unique_ptr<ShaderResource>(new ShaderResource(newId, shaderAsset->info().name, *mBackend));

  // 由asset mgr保证原始数据生命周期
  shaderRsc->setSource(shaderAsset->sources());

  return shaderRsc;
}
}  // namespace pd