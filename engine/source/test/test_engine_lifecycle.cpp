#include "catch2/catch_test_macros.hpp"
#include "pd/engine.hpp"
#include "pd/scene/scene_descriptor.hpp"

namespace pd {
class TestScene : public SceneDescriptor {
 private:
  Result<void> onLoadScene(SceneManager& sceneManager) noexcept override { return {}; };

  void onUpdateScene(float deltaTime) noexcept override {}

  Result<void> onUnloadScene(SceneManager& sceneManager) noexcept override { return {}; }
};
}  // namespace pd

/*
TEST_CASE("test_engine_lifecycle", "engine") {
  using namespace pd;
  EngineConfig config{
      .appName = "test engine",
      .enableDebug = true,
      .platform =
          {
              .window =
                  {
                      .title = "test window",
                      .width = 1024,
                      .height = 768,
                  },
          },
  };
  Engine engine{config};
  auto initResult = engine.initialize();
  REQUIRE(initResult);

  auto runResult = engine.run<TestScene>();
  REQUIRE(runResult);

  auto stopResult = engine.stop();
  REQUIRE(stopResult);

  auto shutdownResult = engine.shutdown();
  REQUIRE(shutdownResult);
}
*/

TEST_CASE("gltf_box", "engine") {
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

  REQUIRE(platform.windowSystem().createWindow());

  BackendConfig backendCfg{
      .windowHandle = platform.windowSystem().nativeWindowHandle(),
      .width = platformCfg.window.width,
      .height = platformCfg.window.height,
      .enableDebug = true,
  };
  Backend backend;
  REQUIRE(backend.init(backendCfg));

  auto& fs = platform.fileSystem();
  AssetManager assetMgr(&fs);

  auto initRes = assetMgr.init();
  REQUIRE(initRes);

  // create shader asset

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

  auto shaderAssetHandle = result.value();
  auto shaderAssetRes = assetMgr.getAsset(shaderAssetHandle);
  REQUIRE(shaderAssetRes);
  auto* pShaderAsset = shaderAssetRes.value();

  // create gltf asset
  Asset::CreateInfo createInfo{
      .name = "box",
      .path = ASSET_DIR "BoxTextured/BoxTextured.gltf",
      .parseType = AssetType::Gltf,
  };

  auto gltfAssetCreateRes = assetMgr.createAsset(createInfo);
  REQUIRE(gltfAssetCreateRes);

  auto gltfAssetRes = assetMgr.getAsset(gltfAssetCreateRes.value());
  REQUIRE(gltfAssetRes);
  auto* pGltfAsset = gltfAssetRes.value();

  // register resource

  ResourceManager rscMgr(&backend);

  auto gltfRegRes = rscMgr.registerAsset<GltfResource_t>(pGltfAsset);
  REQUIRE(gltfRegRes);

  auto gltfResourceHandle = gltfRegRes.value();

  auto shaderRegRes = rscMgr.registerAsset<ShaderResource_t>(pShaderAsset);
  REQUIRE(shaderRegRes);

  auto shaderResourceHandle = shaderRegRes.value();

  // load resources

  auto shaderLoadRes = rscMgr.loadResource(shaderResourceHandle);
  REQUIRE(shaderLoadRes);

  auto gltfLoadRes = rscMgr.loadResource(gltfResourceHandle);
  REQUIRE(gltfLoadRes);

  // todo: create material
  //   MaterialDefinition matDef = MaterialDefinition::Builder()  //
  //                                   .addShaderModule(shaderResourceHandle)
  //                                   .build();
  //   MaterialManager matMgr(&backend);
  //   auto matDefHandle = matMgr.registerMaterial(std::move(matDef));
  //   auto matInstanceHandle = matMgr.createInstance(matDefHandle);

  // create pipeline
  ShaderResource* pShaderResource = rscMgr.getResource(shaderResourceHandle);
  PrefabResource* pGltfResource = rscMgr.getResource(gltfResourceHandle);
  GraphicsPipelineCreateDesc pipelineDesc{
      .debugName = "pyramid",
  };
  pipelineDesc.shaderModules.push_back(pShaderResource->shaderHandle());
  pipelineDesc.shaderPrograms.push_back({
      .shaderModuleIndex = pipelineDesc.shaderModules.size() - 1,
      .stage = ShaderStage::Vertex,
      .entryPoint = "vertMain",
  });
  pipelineDesc.shaderPrograms.push_back({
      .shaderModuleIndex = pipelineDesc.shaderModules.size() - 1,
      .stage = ShaderStage::Fragment,
      .entryPoint = "fragMain",
  });
  // render loop
  CommandRecorder recorder;
  while (!platform.windowSystem().shouldClose()) {
    platform.processEvents();
    // begin frame
    recorder.clear();
    auto frameData = backend.beginFrame();
    recorder.setInfo(frameData.frameIndex, frameData.swapchainImageIndex);
    recorder.addCmd(BeginRenderingArgs{});
    recorder.addCmd(SetViewportArgs{});
    recorder.addCmd(SetScissorArgs{});
    for (const auto meshHandle : pGltfResource->meshes()) {
    }
    // recorder.addCmd(BindPipelineArgs{
    //     .vertexBuffer = vertexBuffer,
    //     .indexBuffer = indexBuffer,
    //     .pipeline = pipelineData.pipeline,
    // });

    // recorder.addCmd(DrawIndexedArgs{
    //     .indexCount = static_cast<u32>(indices.size()),
    //     .instanceCount = 1,
    //     .firstIndex = 0,
    //     .vertexOffset = 0,
    //     .firstInstance = 0,
    // });
    recorder.addCmd(EndRenderingArgs{});
    // end frame
    backend.endFrame(recorder);
  }

  backend.waitIdle();
}