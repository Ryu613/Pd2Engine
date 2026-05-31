#include "catch2/catch_test_macros.hpp"
#include "pd/engine.hpp"
#include "pd/scene/scene_description.hpp"

namespace pd {

class DefaultScene : public SceneDescription {
 public:
  void preRender(AssetManager& assetMgr, TransformManager& transformMgr, Scene& scene,
                 View& view) override {
    /**
     * describe which assets should be used in the scene
     */

    // need a gltf model
    Asset::Info assetInfo{
        .name = "helmet",
        .path = "assets/models/props/DamagedHelmet/DamagedHelmet.gltf",
    };
    Asset* planeAsset = assetMgr.createAsset<Asset::Type::Gltf>(assetInfo);
    // auto assetId = assetManager.Model* plane =
    //     resourceMgr.loadGltfModel("plane0", "assets/models/environment/plane.gltf");

    /**
     * describe which/how objects should be included into the scene
     */
    // Entity planeEntity = scene.addEntity((planeAsset->getRootEntity()));
    // planeEntity.get<Transform>().location = {0.0f, 0.0f, 0.0f};
    // // quaternion
    // planeEntity.get<Transform>().rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    // planeEntity.get<Transform>().scale = {1.0f, 1.0f, 1.0f};

    // set root node
    // scene.setRoot(planeEntity);
    /**
     * TODO: describe which/how lights should be included into the scene
     */

    /**
     * describe how to "look into" the scene
     */

    // setup relations between scene and view(s)
    // view.bindTo(scene);

    // setup view properties
    // view.setAspectRatio();

    // setup the camera(s)
    // Entity camera = view.getMainCamera();

    // setup camera properties
    // camera.setFov()
    // camera.setFocalLength()
    // camera.setZnear()
    // camera.setZFar()
    // camera.setProjection()
    // camera.setExposure()

    // place camera into the scene
    // camera.get<Transform>().location = {0.0f, 0.0f, 0.0f};
  }

  void postRender(AssetManager& assetMgr) override {
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
  try {
    engine->loadScene<pd::DefaultScene>();
    engine->run();
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
  }
}