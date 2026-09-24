#include "catch2/catch_test_macros.hpp"

#include "pd/asset/asset_manager.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/platform/platform.hpp"
#include "pd/backend/backend.hpp"

TEST_CASE("test_shader_resource_register", "engine") {
  using namespace pd;
  PlatformConfig platformCfg{
      .window =
          {
              .width = 1024,
              .height = 768,
          },
  };

  Platform platform(platformCfg);
  REQUIRE(platform.init());
  AssetManager assetMgr{&platform.fileSystem()};

  auto initRes = assetMgr.init();
  REQUIRE(initRes);

  Asset::CreateInfo assetInfo{
      .name = "pyramid shader",
      .path = ASSET_DIR "shader/pyramid/pyramid.slang",
      .parseType = AssetType::Shader,
      .shaderInfo =
          {
              .moduleName = "pyramid",
              .modulePath = "pyramid/pyramid.slang",
          },
  };
  auto result = assetMgr.createAsset(assetInfo);
  REQUIRE(result);

  auto handle = result.value();
  auto assetRes = assetMgr.getAsset(handle);
  REQUIRE(assetRes);
  auto* asset = assetRes.value();
  REQUIRE(asset);

  REQUIRE(platform.windowSystem().createWindow());

  BackendConfig backendCfg{
      .windowHandle = platform.windowSystem().nativeWindowHandle(),
      .width = platformCfg.window.width,
      .height = platformCfg.window.height,
      .enableDebug = true,
  };
  Backend backend;
  REQUIRE(backend.init(backendCfg));

  ResourceManager rscMgr(&backend);

  auto shaderAssetRes = rscMgr.registerAsset<ShaderResource_t>(asset);
  REQUIRE(shaderAssetRes);

  auto shaderHandle = shaderAssetRes.value();

  auto loadResult = rscMgr.loadResource(shaderHandle);
  REQUIRE(loadResult);

  REQUIRE(backend.destroy());
  REQUIRE(platform.destroy());
}

TEST_CASE("test_gltf_resource_register", "engine") {}