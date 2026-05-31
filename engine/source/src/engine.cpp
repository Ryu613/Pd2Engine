#include "pd/engine.hpp"

#include "pd/platform/platform_factory.hpp"

namespace pd {
Engine::Engine(Config& config) noexcept
    : mConfig{config},
      mArena{"Engine Heap Allocator", ResourceType::NullResource{}} {
  log::info("Engine created!");
  log::logo();
}

Engine::~Engine() noexcept { shutdown(); }

bool Engine::initialize() noexcept {
  mPlatform = createPlatform();
  if (!mPlatform->initialize(mConfig.appName, mConfig.windowWidth, mConfig.windowHeight,
                             mConfig.enableDebug)) {
    log::error("platform creation failed!");
    return false;
  }
  mAssetManager =
      std::make_unique<AssetManager>(mPlatform->getFileSystem(), mEntityManager);

  mInitialized = true;
  return true;
}

void Engine::shutdown() noexcept {
  if (!mInitialized) {
    return;
  }
  // explicit destroy subsystem
  mRenderer.reset();
  mAssetManager.reset();
  mPlatform.reset();
}

void Engine::run() {
  if (!mInitialized) {
    throw std::runtime_error("Engine not initialized!");
  }

  while (!mPlatform->shouldClose()) {
    mPlatform->processEvents();

    mRenderer->beginFrame();
    // mRenderer->renderFrame();
    mRenderer->endFrame();
  }
}
}  // namespace pd