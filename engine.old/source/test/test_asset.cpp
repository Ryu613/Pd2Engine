#include "catch2/catch_test_macros.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/platform/file/std_file_system.hpp"
#include "pd/asset/asset_manager.hpp"

TEST_CASE("test_gltf_asset", "asset") {
  using namespace pd;
  StdFileSystem fileSystem;
  ResourceManager resourceMgr;
  AssetManager assetMgr{&fileSystem, &resourceMgr};

  pd::Asset::CreateInfo assetInfo{
      .name = "box",
      .path = "assets/models/props/BoxTextured/BoxTextured.glb",
      .parseType = pd::Asset::Type::Gltf,
  };
  auto result = assetMgr.createAsset(assetInfo);
  REQUIRE(result);

  auto* pAsset = result.value();

  // 创建重复资源
  auto result2 = assetMgr.createAsset(assetInfo);
  REQUIRE(result2);

  auto* pAsset2 = result2.value();
  REQUIRE(pAsset == pAsset2);
}