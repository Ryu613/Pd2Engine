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
  LOG_INFO("Engine created!");

  initContext(mArena);

  auto platformInitRes = mPlatform.init();
  PD_ASSERT_MSG(platformInitRes, platformInitRes.error().msg.data());

  log::logo();
}

Engine::~Engine() noexcept { shutdown(); }

Result<void> Engine::shutdown() noexcept {
  if (!mInitialized) {
    return {};
  }
  LOG_INFO("shutdown engine");
  if (auto res = mRenderer.destroy(); !res) {
    PD_ASSERT(res.error().msg.data());
  }
  if (auto res = mSceneManager.destroy(); !res) {
    PD_ASSERT(res.error().msg.data());
  }
  if (auto res = mAssetManager.destroy(); !res) {
    PD_ASSERT(res.error().msg.data());
  }
  if (auto res = mBackend.destroy(); !res) {
    PD_ASSERT(res.error().msg.data());
  }
  if (auto res = mPlatform.destroy(); !res) {
    PD_ASSERT(res.error().msg.data());
  }

  global::destroyContext();

  mInitialized = false;
  return {};
}

Result<void> Engine::initialize() noexcept { return {}; }

Result<void> Engine::run() noexcept { return {}; }

Result<void> Engine::stop() noexcept { return {}; }

}  // namespace pd