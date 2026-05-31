#include "pd/asset/asset_manager.hpp"

#include "pd/asset/parser/gltf_parser.hpp"

namespace pd {
AssetManager::AssetManager(FileSystem& fileSystem, EntityManager& entityManager) noexcept
    : mFileSystem(fileSystem),
      mEntityManger(entityManager) {
  initParsers();
}

void AssetManager::initParsers() noexcept {
  auto gltfParser = std::make_unique<GltfParser>();
  mParsers.emplace(Asset::Type::Gltf, std::move(gltfParser));
}
}  // namespace pd