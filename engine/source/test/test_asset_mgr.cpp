#include "catch2/catch_test_macros.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/platform/fs/std_file_system.hpp"

#ifndef ASSET_DIR
#define ASSET_DIR ./
#endif

TEST_CASE("test_glb_asset", "asset") {
  using namespace pd;
  StdFileSystem fileSystem;
  AssetManager assetMgr{fileSystem};

  auto initRes = assetMgr.init();
  REQUIRE(initRes);

  pd::Asset::MetaInfo assetInfo{
      .name = "box",
      .path = ASSET_DIR "BoxTextured/BoxTextured.glb",
      .parseType = ParseType::Gltf,
  };
  auto result = assetMgr.createAsset(assetInfo);
  REQUIRE(result);

  auto& gltfAssetHandle = result.value();

  auto getAssetRes = assetMgr.getAsset(gltfAssetHandle);
  REQUIRE(getAssetRes);
  GltfAsset* pAsset = static_cast<GltfAsset*>(getAssetRes.value());
  REQUIRE(pAsset->info().name == "box");
  REQUIRE(pAsset->nodes().size() == 1);

  auto& node = pAsset->nodes().front();
  REQUIRE(node.meshId == 0);
  auto meshes = pAsset->meshes();
  auto& meshData = meshes[node.meshId];
  REQUIRE(meshData.primitives.size() == 1);
  auto& primitives = meshData.primitives;
  auto& primitive = primitives[0];
  REQUIRE(primitive.vertices.size() == 24);
  REQUIRE(primitive.indices.size() == 36);
  REQUIRE(primitive.materialIndex == 0);
  auto& material = pAsset->materials()[primitive.materialIndex];
  REQUIRE(material.baseColorTextureIndex == 0);
  auto& texture = pAsset->textures()[material.baseColorTextureIndex];
  REQUIRE(texture.imageIndex == 0);
  REQUIRE(pAsset->images().size() == 1);
  auto& image = pAsset->images()[texture.imageIndex];
  REQUIRE(image.bytes.size() > 0);
}

TEST_CASE("test_shader_asset", "asset") {
  using namespace pd;
  StdFileSystem fileSystem;
  AssetManager assetMgr{fileSystem};

  auto initRes = assetMgr.init();
  REQUIRE(initRes);

  Asset::MetaInfo assetInfo{
      .name = "pyramid shader",
      .path = ASSET_DIR "shader/pyramid/pyramid.slang",
      .parseType = ParseType::Shader,
      .shaderInfo =
          Asset::ShaderInfo{
              .moduleName = "pyramid",
              .modulePath = "pyramid/pyramid.slang",
          },
  };
  auto result = assetMgr.createAsset(assetInfo);
  REQUIRE(result);

  auto handle = result.value();
  auto assetRes = assetMgr.getAsset(handle);
  REQUIRE(assetRes);
  ShaderAsset* asset = static_cast<ShaderAsset*>(assetRes.value());
  REQUIRE(asset->sources().size() > 0);
}