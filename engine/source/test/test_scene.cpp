#include "catch2/catch_test_macros.hpp"
#include <array>
#include "pd/engine.hpp"
#include "pd/scene/scene_descriptor.hpp"

namespace pd {

class TriangleScene : public SceneDescriptor {
 private:
  Result<void, Error::Scene> onLoadScene(SceneManager& sceneManager) noexcept override {
    std::array<Vertex, 3> vertices{
        Vertex{
            .position = {-1.0f, -1.0f, 0.0f},
            .color = {1.0f, 0.0f, 0.0f},
        },
        Vertex{
            .position = {0.0f, 1.0f, 0.0f},
            .color = {0.0f, 1.0f, 0.0f},
        },
        Vertex{
            .position = {1.0f, -1.0f, 0.0f},
            .color = {0.0f, 0.0f, 1.0f},
        },
    };
    std::array<uint32_t, 3> indices{0, 1, 2};
    MeshPrimitive primitive;
    primitive.indices.assign(indices.begin(), indices.end());
    primitive.vertices.assign(vertices.begin(), vertices.end());
    MeshResource mesh;
    mesh.addPrimitive(primitive);
    auto& resourceMgr = sceneManager.getResourceManager();
    auto meshHandle = resourceMgr.registerResource<MeshResource_t>(std::move(mesh));
    auto& entityMgr = sceneManager.getEntityManager();
    auto entity = entityMgr.createEntity();
    Transform tr;
    entityMgr.addComponent<Transform>(entity, tr);
    // auto& transformMgr = sceneManager.getTransformManager();
    // transformMgr.create(entity, tr);
    Renderable renderable;
    renderable.meshHandle = meshHandle;
    entityMgr.addComponent<Renderable>(entity, renderable);
    auto& scene = sceneManager.getScene();
    scene.addEntity(entity);
    // auto& view = scene.createView();

    return {};
  };

  void onUpdateScene() noexcept override {}

  Result<void, Error::Scene> onUnloadScene() noexcept override { return {}; }
};

}  // namespace pd

TEST_CASE("test_scene", "engine") {
  using pd::Engine;
  Engine::Config config{
      .appName = "test engine",
      .enableDebug = true,
  };
  auto engine = std::make_unique<Engine>(config);
  auto engineInitResult = engine->initialize();
  REQUIRE(engineInitResult);

  auto sceneCreateResult = engine->loadScene<pd::TriangleScene>();
  if (!sceneCreateResult) {
    pd::log::error("scene load failed: {}", sceneCreateResult.error());
    REQUIRE(false);
  }
  auto runResult = engine->run();
}