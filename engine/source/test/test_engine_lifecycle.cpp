#include "catch2/catch_test_macros.hpp"
#include "pd/engine.hpp"

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
  //   auto initResult = engine->initialize();
  //   REQUIRE(initResult);

  //   auto runResult = engine->run();
  //   REQUIRE(runResult);
  auto shutdownResult = engine.shutdown();
  REQUIRE(shutdownResult);
}