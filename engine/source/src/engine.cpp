#include "pd/engine.hpp"

namespace pd {

namespace {
void initContext(GlobalAllocator& allocator) noexcept { global::initContext(allocator); }

}  // namespace

Engine::Engine(EngineConfig config) noexcept
    : mConfig{std::move(config)},
      mArena{"Engine Heap Allocator", ResourceType::NullResource{}},
      mPlatform(mConfig.platform),
      mResourceManager(&mBackend),
      mAssetManager(&mPlatform.fileSystem()),
      mSceneManager(&mResourceManager, &mAssetManager),
      mRenderer(&mSceneManager, &mBackend) {
  // init common services
  log::init();

  // init global services
  initContext(mArena);

  // platform must be initialized before other sub systems
  auto platformInitRes = mPlatform.init();
  PD_ASSERT_MSG(platformInitRes, platformInitRes.error().msg.data());

  log::logo();
  LOG_INFO("Engine created!");
}

Engine::~Engine() noexcept {
  if (auto res = shutdown(); !res) {
    LOG_ERROR(res.error().msg);
  }
}

Result<void> Engine::shutdown() noexcept {
  if (!mInitialized) {
    return {};
  }
  LOG_INFO("shutdown engine");

  if (auto res = mRenderer.destroy(); !res) {
    return res;
  }
  if (auto res = mSceneManager.destroy(); !res) {
    return res;
  }
  if (auto res = mResourceManager.destroy(); !res) {
    return res;
  }
  if (auto res = mAssetManager.destroy(); !res) {
    return res;
  }
  if (auto res = mBackend.destroy(); !res) {
    return res;
  }
  if (auto res = mPlatform.destroy(); !res) {
    return res;
  }

  global::destroyContext();

  mInitialized = false;
  return {};
}

Result<void> Engine::initialize() noexcept {
  if (auto res = mPlatform.windowSystem().createWindow(); !res) {
    return res;
  }
  BackendConfig backendCfg{
      .windowHandle = mPlatform.windowSystem().nativeWindowHandle(),
      .width = mConfig.platform.window.width,
      .height = mConfig.platform.window.height,
      .enableDebug = mConfig.enableDebug,
  };
  if (auto res = mBackend.init(backendCfg); !res) {
    LOG_ERROR(res.error().msg);
    return res;
  }
  if (auto res = mResourceManager.init(); !res) {
    LOG_ERROR(res.error().msg);
    return res;
  }
  if (auto res = mAssetManager.init(); !res) {
    LOG_ERROR(res.error().msg);
    return res;
  }
  if (auto res = mSceneManager.init(); !res) {
    LOG_ERROR(res.error().msg);
    return res;
  }
  if (auto res = mRenderer.init(); !res) {
    LOG_ERROR(res.error().msg);
    return res;
  }
  mInitialized = true;
  return {};
}

void Engine::runImpl() noexcept {
  // load scene
  if (auto res = mSceneManager.loadScene(); !res) {
    PD_ASSERT_MSG(res, res.error().msg.data());
  }
  loop();
  auto engineStopRes = stop();
  PD_ASSERT(engineStopRes);
}

void Engine::loop() noexcept {
  while (!mPlatform.windowSystem().shouldClose()) {
    mPlatform.processEvents();
    mSceneManager.updateScene(0);

    mRenderer.renderFrame();
  }
}

Result<void> Engine::stop() noexcept {
  if (auto res = mSceneManager.unloadScene(); !res) {
    LOG_ERROR(res.error().msg);
    return res;
  }
  return {};
}

}  // namespace pd