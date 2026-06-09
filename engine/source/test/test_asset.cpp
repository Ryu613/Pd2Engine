#include "catch2/catch_test_macros.hpp"
#include "pd/core/entity_manager.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/platform/file/std_file_system.hpp"
#include "pd/asset/asset_manager.hpp"

TEST_CASE("test_gltf_asset", "asset") {
  auto fileSystem = std::make_unique<pd::StdFileSystem>();
  auto entityManager = std::make_unique<pd::EntityManager>();
  auto resourceManager = std::make_unique<pd::ResourceManager>();
  auto assetManager =
      std::make_unique<pd::AssetManager>(*fileSystem, *entityManager, *resourceManager);

  pd::Asset::Info assetInfo{
      .name = "box",
      .path = "assets/models/props/BoxTextured/BoxTextured.glb",
      .parseType = pd::Asset::Type::Gltf,
  };
  auto result = assetManager->createAsset(assetInfo);
  REQUIRE(result);

  pd::Entity e = result.value()->getRootEntity();
}