#include "pd/engine.hpp"

#include "world.hpp"

int main() {
  pd::EngineConfig config{
      .appName = "sandbox",
      .enableDebug = true,
  };
  pd::Engine engine(config);
  auto initResult = engine.initialize();
  if (!initResult) {
    LOG_ERROR(initResult.error().msg);
    return -1;
  }
  auto loadResult = engine.loadScene<sandbox::WorldScene>();
  if (!loadResult) {
    LOG_ERROR(loadResult.error().msg);
    return -1;
  }
  auto runResult = engine.run();
  if (!runResult) {
    LOG_ERROR(runResult.error().msg);
    return -1;
  }

  return 0;
}