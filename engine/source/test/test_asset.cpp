#include "catch2/catch_test_macros.hpp"
#include "pd/core/entity_manager.hpp"
#include "pd/platform/file/std_file_system.hpp"
#include "pd/asset/asset_manager.hpp"

TEST_CASE("test_gltf_asset", "asset") {
  auto fileSystem = std::make_unique<pd::StdFileSystem>();
  auto entityManager = std::make_unique<pd::EntityManager>();
  auto assetManager = std::make_unique<pd::AssetManager>(*fileSystem, *entityManager);

  pd::Asset::Info assetInfo{
      .name = "plane",
      .path = "assets/models/props/BoxTextured/BoxTextured.glb",
  };
  assetManager->createAsset<pd::Asset::Type::Gltf>(assetInfo);
}