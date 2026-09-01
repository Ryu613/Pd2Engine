#include "pd/app/application.hpp"

namespace pd::app {

Application::Application(AppConfig config)
    : mConfig(std::move(config)),
      mEngine(mConfig.engineConfig) {}

Application::~Application() {}

void Application::close() {
  // app关闭
  shutdown();
  // engine关闭
  mEngine.shutdown();
}

void Application::shutdown() {}
}  // namespace pd::app