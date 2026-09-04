#pragma once

#include "pd/app/config.hpp"
#include "pd/engine.hpp"

namespace pd {
namespace app {
class Application {
 public:
  Application(AppConfig config);
  ~Application();

  Application(const Application& rhs) = delete;
  Application& operator=(const Application& rhs) = delete;
  Application(Application&& rhs) noexcept = delete;
  Application& operator=(Application&& rhs) noexcept = delete;

  template <typename T, typename... Args>
  void run(Args&&... args);

  void stop();

 private:
  AppConfig mConfig;
  Engine mEngine;
  bool mInitialized = false;

  void shutdown();
};

template <typename T, typename... Args>
inline void Application::run(Args&&... args) {
  // 引擎初始化
  auto engineInitRes = mEngine.initialize();
  PD_ASSERT_MSG(engineInitRes, "engine init failed");
  // 加载场景
  // mEngine.loadScene<T>(std::forward<Args>(args)...);
  // 开始渲染
  // auto engineRunRes = mEngine.run();
  // PD_ASSERT_MSG(engineRunRes, "engine run failed");
}
}  // namespace app
}  // namespace pd