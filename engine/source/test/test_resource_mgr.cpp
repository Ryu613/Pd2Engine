#include "catch2/catch_test_macros.hpp"

#include "pd/asset/asset_manager.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/platform/platform.hpp"
#include "pd/backend/backend.hpp"

/*
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

  auto unloadResult = rscMgr.unloadResource(shaderHandle);

  REQUIRE(backend.destroy());
  REQUIRE(platform.destroy());
}
*/

TEST_CASE("test_gltf_register", "resource") {
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
  AssetManager assetMgr{platform.fileSystem()};

  auto initRes = assetMgr.init();
  REQUIRE(initRes);

  Asset::MetaInfo assetInfo{
      .name = "Box Textured",
      .path = ASSET_DIR "BoxTextured/BoxTextured.gltf",
      .parseType = ParseType::Gltf,
  };
  auto result = assetMgr.createAsset(assetInfo);
  REQUIRE(result);

  auto handle = result.value();
  auto assetRes = assetMgr.getAsset(handle);
  REQUIRE(assetRes);
  const auto* asset = assetRes.value();
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

  ResourceManager rscMgr(backend);

  SECTION("one prefab") {
    auto gltfAssetRefRes = rscMgr.registerResourceFromAsset<GltfAsset, PrefabResource_t>(ResourceCreateInfo<GltfAsset>{
        .pAsset = static_cast<const GltfAsset*>(asset),
        .variantId = 0,
    });
    REQUIRE(gltfAssetRefRes);
    auto& gltfRef = gltfAssetRefRes.value();
    auto gltfHandle = gltfRef.handle();
    auto [prefabEntry, prefabPool] = rscMgr.getResourceDebugView<PrefabResource>(gltfHandle);
    REQUIRE(prefabEntry.refCount == 1);

    // first time load
    auto loadRes = gltfRef.load();
    REQUIRE(loadRes);
    REQUIRE(prefabEntry.loadCount == 1);

    auto* prefab = prefabPool.get(gltfHandle);
    REQUIRE(prefab->status() == ResourceStatus::Loaded);
    // load again
    loadRes = gltfRef.load();
    REQUIRE(loadRes);
    REQUIRE(prefabEntry.loadCount == 2);
    REQUIRE(prefabEntry.refCount == 1);

    // copy
    auto gltfRef2 = gltfRef;
    REQUIRE(prefabEntry.refCount == 2);
    auto load2Res = gltfRef2.load();
    REQUIRE(loadRes);
    REQUIRE(prefabEntry.loadCount == 3);

    // unload
    auto unloadRes = gltfRef.unload();
    REQUIRE(unloadRes);
    REQUIRE(prefabEntry.loadCount == 2);
    REQUIRE(prefab->status() == ResourceStatus::Loaded);
    unloadRes = gltfRef2.unload();
    REQUIRE(unloadRes);
    REQUIRE(prefabEntry.loadCount == 1);
    REQUIRE(prefab->status() == ResourceStatus::Loaded);
    unloadRes = gltfRef.unload();
    REQUIRE(unloadRes);
    REQUIRE(prefabEntry.loadCount == 0);
    REQUIRE(prefab->status() == ResourceStatus::Unload);
    unloadRes = gltfRef.unload();
    REQUIRE(unloadRes);
    REQUIRE(prefabEntry.loadCount == 0);
    REQUIRE(prefab->status() == ResourceStatus::Unload);

    // move
    {
      auto gltfRef3 = std::move(gltfRef);
      auto gltfHandle = gltfRef3.handle();
      auto acquireRes = rscMgr.acquire(gltfHandle);
      REQUIRE(acquireRes);
      REQUIRE(prefabEntry.refCount == 3);
    }  // ~gltfRef2()

    REQUIRE(prefabEntry.refCount == 1);

    gltfRef.reset(); // gltfRef moved, not working(intended)
    REQUIRE(prefabEntry.refCount == 1);
    gltfRef2.reset();
    REQUIRE(prefabEntry.refCount == 0);

    gltfRef.reset();    // not working(intended) 
    loadRes = gltfRef.load(); // not working(intended)
  }

  SECTION("two prefab with same asset") {
    // two
    auto gltfAssetRef1Res = rscMgr.registerResourceFromAsset<GltfAsset, PrefabResource_t>(ResourceCreateInfo<GltfAsset>{
        .pAsset = static_cast<const GltfAsset*>(asset),
        .variantId = 1,
    });
    REQUIRE(gltfAssetRef1Res);
    auto gltfRef1 = gltfAssetRef1Res.value();
    auto loadRes = gltfRef1.load();
    REQUIRE(loadRes);
  }

  REQUIRE(rscMgr.destroy());

  REQUIRE(backend.destroy());
  REQUIRE(platform.destroy());
}