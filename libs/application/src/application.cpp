#include "pd/app/application.hpp"

namespace pd::app {

Application::Application(AppConfig config)
    : mConfig(std::move(config)),
      mEngine(mConfig.engineConfig) {}

Application::~Application() { shutdown(); }

void Application::stop() {
  // engine关闭
  auto engineStopRes = mEngine.stop();
  PD_ASSERT_MSG(engineStopRes, "engine stop failed!");
}

void Application::shutdown() {
  if (!mInitialized) {
    return;
  }
  auto engineShutdownRes = mEngine.shutdown();
  PD_ASSERT_MSG(engineShutdownRes, "engine shutdown faield!");
}
}  // namespace pd::app