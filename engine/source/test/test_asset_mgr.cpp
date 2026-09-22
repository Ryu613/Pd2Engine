#include "catch2/catch_test_macros.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/platform/fs/std_file_system.hpp"

#ifndef ASSET_DIR
#define ASSET_DIR ./
#endif

TEST_CASE("test_gltf_asset", "engine") {
  using namespace pd;
  StdFileSystem fileSystem;
  AssetManager assetMgr{&fileSystem};

  auto initRes = assetMgr.init();
  REQUIRE(initRes);

  pd::Asset::CreateInfo assetInfo{
      .name = "box",
      .path = ASSET_DIR "BoxTextured/BoxTextured.glb",
      .parseType = AssetType::Gltf,
  };
  auto result = assetMgr.createAsset(assetInfo);
  REQUIRE(result);
}

TEST_CASE("test_shader_asset", "engine") {
  using namespace pd;
  StdFileSystem fileSystem;
  AssetManager assetMgr{&fileSystem};

  auto initRes = assetMgr.init();
  REQUIRE(initRes);

  Asset::CreateInfo assetInfo{
      .name = "pyramid shader",
      .path = ASSET_DIR "shader/pyramid/pyramid.slang",
      .parseType = AssetType::Shader,
  };
  auto result = assetMgr.createAsset(assetInfo);
  REQUIRE(result);

  auto handle = result.value();
  auto assetRes = assetMgr.getAsset(handle);
  REQUIRE(assetRes);
  auto* asset = assetRes.value();
  REQUIRE(asset);
}