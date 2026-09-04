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