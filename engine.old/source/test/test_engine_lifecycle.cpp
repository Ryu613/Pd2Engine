#include "catch2/catch_test_macros.hpp"
#include "pd/engine.hpp"

TEST_CASE("test_engine_lifecycle", "engine") {
  using pd::Engine;
  Engine::Config config{
      .appName = "test engine",
      .enableDebug = true,
  };
  auto engine = std::make_unique<Engine>(config);
  auto initResult = engine->initialize();
  REQUIRE(initResult);

  auto runResult = engine->run();
  REQUIRE(runResult);
}