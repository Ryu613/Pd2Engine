#include "pd/engine.hpp"

namespace pd {

namespace {
void initContext(GlobalAllocator& allocator) noexcept { global::initContext(allocator); }

template <typename T>
Result<T> setErrorStatus(EngineStatus& status, Result<T> res) noexcept {
  status = EngineStatus::Error;
  LOG_ERROR(res.error().msg);
  return res;
}

}  // namespace

Engine::Engine(EngineConfig config) noexcept
    : mConfig{std::move(config)},
      mStatus(EngineStatus::Stopped),
      mArena{"Engine Heap Allocator", ResourceType::NullResource{}},
      mPlatform(mConfig.platform),
      mResourceManager(mBackend),
      mAssetManager(mPlatform.fileSystem()),
      mSceneManager(&mResourceManager, &mAssetManager),
      mRenderer(&mSceneManager, &mBackend) {
  // init common services
  log::init();

  // init global services
  initContext(mArena);

  log::logo();
  LOG_INFO("Engine created!");

  mStatus = EngineStatus::Constructed;
}

Engine::~Engine() noexcept {
  if (auto res = shutdown(); !res) {
    std::terminate();
  }
}

Result<void> Engine::shutdown() noexcept {
  LOG_INFO("shutdown engine");
  if (mStatus == EngineStatus::Shutdown) {
    return {};
  } else if (mStatus == EngineStatus::Constructed) {
    global::destroyContext();
    mStatus = EngineStatus::Shutdown;
    return {};
  } else {
    if (auto res = requestStop(); !res) {
      std::terminate();
    }
    if (auto res = mRenderer.destroy(); !res) {
      return setErrorStatus(mStatus, res);
    }
    if (auto res = mSceneManager.destroy(); !res) {
      return setErrorStatus(mStatus, res);
    }
    if (auto res = mResourceManager.destroy(); !res) {
      return setErrorStatus(mStatus, res);
    }
    if (auto res = mAssetManager.destroy(); !res) {
      return setErrorStatus(mStatus, res);
    }
    if (auto res = mBackend.destroy(); !res) {
      return setErrorStatus(mStatus, res);
    }
    if (auto res = mPlatform.destroy(); !res) {
      return setErrorStatus(mStatus, res);
    }

    global::destroyContext();
    mStatus = EngineStatus::Shutdown;
  }

  return {};
}

Result<void> Engine::initialize() noexcept {
  PD_ASSERT(mStatus == EngineStatus::Constructed);
  if (auto res = mPlatform.init(); !res) {
    return setErrorStatus(mStatus, res);
    ;
  }
  if (auto res = mPlatform.windowSystem().createWindow(); !res) {
    return setErrorStatus(mStatus, res);
    ;
  }
  BackendConfig backendCfg{
      .windowHandle = mPlatform.windowSystem().nativeWindowHandle(),
      .width = mConfig.platform.window.width,
      .height = mConfig.platform.window.height,
      .enableDebug = mConfig.enableDebug,
  };
  if (auto res = mBackend.init(backendCfg); !res) {
    return setErrorStatus(mStatus, res);
  }
  if (auto res = mResourceManager.init(); !res) {
    return setErrorStatus(mStatus, res);
  }
  if (auto res = mAssetManager.init(); !res) {
    return setErrorStatus(mStatus, res);
  }
  if (auto res = mSceneManager.init(); !res) {
    return setErrorStatus(mStatus, res);
  }
  if (auto res = mRenderer.init(); !res) {
    return setErrorStatus(mStatus, res);
  }
  mStatus = EngineStatus::Initialized;
  return {};
}

Result<void> Engine::runImpl() noexcept {
  PD_ASSERT(mStatus == EngineStatus::Initialized);
  // load scene
  if (auto res = mSceneManager.loadScene(); !res) {
    return setErrorStatus(mStatus, res);
  }
  mStatus = EngineStatus::Running;
  loop();
  return {};
}

void Engine::loop() noexcept {
  auto& windowSystem = mPlatform.windowSystem();
  while (!windowSystem.shouldClose() && mStatus == EngineStatus::Running) {
    mPlatform.processEvents();
    tick();
  }
  auto res = requestStop();
  PD_ASSERT(res);
  // auto engineStopRes = stop();
  // PD_ASSERT_MSG(engineStopRes, engineStopRes.error().msg.data());
}

Result<void> Engine::requestStop() noexcept {
  if (mStatus != EngineStatus::Running) {
    return {};
  }
  mStatus = EngineStatus::Stopping;
  if (auto res = stop(); !res) {
    return setErrorStatus(mStatus, res);
  }
  return {};
}

Result<void> Engine::stop() noexcept {
  PD_ASSERT(mStatus == EngineStatus::Stopping);
  // reset scene data, status
  if (auto res = mSceneManager.unloadScene(); !res) {
    return setErrorStatus(mStatus, res);
  }
  // todo: clear cached resources
  // todo: clear cached materials
  // todo: clear cached assets
  mStatus = EngineStatus::Stopped;
  return {};
}

void Engine::tick() noexcept {
  mSceneManager.updateScene(0);
  mRenderer.renderFrame();
}

EngineStatus Engine::status() const noexcept { return mStatus; }

}  // namespace pd