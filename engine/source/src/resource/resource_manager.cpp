#include "pd/resource/resource_manager.hpp"

#include "pd/asset/asset.hpp"

namespace pd {
ResourceManager::ResourceManager(Backend* backend)
    : mBackend(backend) {}

ResourceManager::~ResourceManager() {}

Result<void> ResourceManager::init() noexcept { return {}; }

Result<void> ResourceManager::destroy() noexcept { return {}; }

Result<void> ResourceManager::createResourcesFromAsset(Asset* asset) noexcept {
  PD_ASSERT_MSG(asset, "asset pointer is null!");
  switch (asset->info().parseType) {
    using enum AssetType;
    case Gltf:
      if (auto res = registerGltfAsset(static_cast<GltfAsset*>(asset)); !res) {
        return res;
      }
      break;
    case Shader:
      if (auto res = registerShaderAsset(static_cast<ShaderAsset*>(asset)); !res) {
        return res;
      }
      break;
    default:
      return make_error<void>(ErrorCode::ResourceTypeNotSupported);
  }

  return {};
}

Result<void> ResourceManager::registerGltfAsset(GltfAsset* gltfAsset) noexcept {
  LOG_INFO("registering gltf resource {}", gltfAsset->info().name);
  return {};
}
Result<void> ResourceManager::registerShaderAsset(ShaderAsset* shaderAsset) noexcept {
  LOG_INFO("registering shader resource {}", shaderAsset->info().name);
  auto regIt = mRegistry.find(shaderAsset->id());
  if (regIt != mRegistry.end()) {
    return {};
  }

  auto newId = nextId<ShaderResource_t>();
  auto shaderRsc = std::unique_ptr<ShaderResource>(new ShaderResource(newId, shaderAsset->info().name, *mBackend));

  auto [regInsIt, regSuccess] = mRegistry.emplace(shaderAsset->id(), ResourceEntry{
                                                                         .handle =
                                                                             {
                                                                                 .id = newId,
                                                                                 .gen = 0,
                                                                             },
                                                                         .refCount = 0,
                                                                     });
  PD_ASSERT_MSG(regSuccess, "shader resource register failed!");

  auto [shaderInsIt, shaderInsSuccess] = mShaders.emplace(newId, std::move(shaderRsc));
  PD_ASSERT_MSG(shaderInsSuccess, "shader resource insert failed!");
  return {};
}
}  // namespace pd