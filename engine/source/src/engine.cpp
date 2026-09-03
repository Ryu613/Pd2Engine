#include "pd/engine.hpp"

namespace pd {

namespace {
void initContext(GlobalAllocator& allocator) noexcept { global::initContext(allocator); }

}  // namespace

Engine::Engine(EngineConfig config) noexcept
    : mConfig{std::move(config)},
      mArena{"Engine Heap Allocator", ResourceType::NullResource{}},
      mPlatform(mConfig.platform) {
  log::init();
  LOG_INFO("Engine created!");
  initContext(mArena);
  mPlatform.init();

  log::logo();
}

Engine::~Engine() noexcept { shutdown(); }

Result<void> Engine::shutdown() noexcept {
  if (!mInitialized) {
    return {};
  }

  global::destroyContext();

  mInitialized = false;
  return {};
}

}  // namespace pd