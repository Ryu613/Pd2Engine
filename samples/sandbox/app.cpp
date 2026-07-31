#include "pd/engine.hpp"

#include "world.hpp"

int main() {
  pd::EngineConfig config{
      .appName = "sandbox",
      .enableDebug = true,
  };
  pd::Engine engine(config);
  auto initResult = engine.initialize();
  auto loadResult = engine.loadScene<sandbox::WorldScene>();
  auto runResult = engine.run();
}