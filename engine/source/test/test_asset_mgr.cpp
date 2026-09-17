#include "catch2/catch_test_macros.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/platform/fs/std_file_system.hpp"

#ifndef ASSET_BASE_DIR
#define ASSET_BASE_DIR ./
#endif

TEST_CASE("test_asset_loading", "engine") {
  using namespace pd;
  StdFileSystem fileSystem;
  AssetManager assetMgr{&fileSystem};

  pd::Asset::CreateInfo assetInfo{
      .name = "box",
      .path = ASSET_BASE_DIR "BoxTextured/BoxTextured.glb",
      .parseType = AssetType::Gltf,
  };
  auto result = assetMgr.createAsset(assetInfo);
  REQUIRE(result);
}