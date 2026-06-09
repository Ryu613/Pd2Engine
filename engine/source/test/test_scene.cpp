#include "catch2/catch_test_macros.hpp"
#include "pd/engine.hpp"
#include "pd/scene/scene_descriptor.hpp"

#include <print>

namespace pd {

class DefaultScene : public SceneDescriptor {
 public:
  void onLoad(AssetManager& assetMgr, EntityManager& entityMgr,
              TransformManager& transformMgr, CameraManager& cameraMgr,
              Scene& scene) override {
    /**
     * describe which assets should be used in the scene
     */

    // need a gltf model
    pd::Asset::Info assetInfo{
        .name = "box",
        .path = "assets/models/props/BoxTextured/BoxTextured.glb",
        .parseType = pd::Asset::Type::Gltf,
    };
    auto result = assetMgr.createAsset(assetInfo);
    REQUIRE(result);

    /**
     * describe which/how objects should be included into the scene
     */

    // use gltf's scene graph root node as Scene entity
    auto& asset = result.value();
    auto sceneRootEntity = asset->getRootEntity();
    scene.addEntity(sceneRootEntity);

    // set scene root transformation
    auto* rootTransform = transformMgr.getEntity(sceneRootEntity);
    auto& transform = *rootTransform;
    transform.location = {0.0f, 0.0f, 0.0f};
    transform.rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    transform.scale = {1.0f, 1.0f, 1.0f};
    // create and configure scene view
    auto& view = scene.createView();
    Entity cameraEntity = entityMgr.createEntity();
    auto* camera = cameraMgr.create(cameraEntity);
    camera->hFov = 0.75f;
    auto* cameraTransform = transformMgr.create(cameraEntity);
    cameraTransform->location = {0.0f, 0.0f, 0.0f};

    view.setMainCamera(cameraEntity);
  }

  void onUnload(Scene& scene) override {
    // clear resource which required before rendering
    // for (auto handle : mAssetHandles) {
    // resourceMgr.release(handle);
    // }
  }
};

}  // namespace pd

TEST_CASE("test_scene", "engine") {
  using pd::Engine;
  Engine::Config config{
      .appName = "test engine",
      .enableDebug = true,
  };
  auto engine = std::make_unique<Engine>(config);
  engine->initialize();
  auto loadResult = engine->loadScene<pd::DefaultScene>();
  if (!loadResult) {
    pd::log::error("scene load failed: {}", loadResult.error());
  }
  auto runResult = engine->run();
}