#include "pd/engine.hpp"

#include "pd/platform/platform_factory.hpp"
#include "pd/backend/backend_factory.hpp"

namespace pd {

namespace {
void initContext(GlobalAllocator& allocator) noexcept { global::initContext(allocator); }

Renderer initRenderer(const EngineConfig& config) noexcept {
  return Renderer(Renderer::Config{});
}

}  // namespace

Engine::Engine(EngineConfig config) noexcept
    : mConfig{std::move(config)},
      mArena{"Engine Heap Allocator", ResourceType::NullResource{}},
      mRenderer(initRenderer(mConfig)) {
  LOG_INFO("Engine created!");
  initContext(mArena);
  mLayers.reserve(4);
  log::logo();
}

Engine::~Engine() noexcept { shutdown(); }

Result<void> Engine::initialize() noexcept {
  // 初始化平台层
  IPlatform::Config config{
      .window =
          {
              .title = mConfig.appName,
              .width = mConfig.windowWidth,
              .height = mConfig.windowHeight,
          },
  };
  mPlatform = createPlatform(config);

  // 创建窗口
  if (!mPlatform->window()->create()) {
    LOG_ERROR("window creation failed!");
    return make_error<void>(ErrorCode::EngineInitializeFailed);
  }
  // 初始化渲染后端
  auto* pWindow = mPlatform->window();
  IBackend::Config backendConfig{
      .pWindow = pWindow,
  };
  mBackend = createBackend(backendConfig);

  // 初始化其他子系统
  mResourceManager.initialize(mBackend.get());
  mSceneManager.initialize(&mAssetManager, &mResourceManager);
  mRenderer.initialize(mBackend.get(), pWindow, &mSceneManager, &mResourceManager);

  mInitialized = true;
  return {};
}

void Engine::shutdown() noexcept {
  if (!mInitialized) {
    return;
  }
  for (auto& layer : mLayers) {
    auto result = layer->onDetached();
    if (!result) {
      LOG_ERROR("layer detach failed! layer: {}, error: {}", layer->name(),
                result.error().msg);
      continue;
    }
  }

  global::destroyContext();

  mInitialized = false;
}

Result<void> Engine::run() noexcept {
  if (!mInitialized) {
    return make_error<void>(ErrorCode::EngineRunFailed);
  }
  // prepare scene data
  auto sceneLoadResult = mSceneManager.loadScene();
  if (!sceneLoadResult) {
    LOG_ERROR("scene load failed!, error: {}", sceneLoadResult.error().msg);
    return sceneLoadResult;
  }

  while (!mPlatform->window()->shouldClose()) {
    mPlatform->processEvents();
    // TODO(ryu613): use event system instead of running in main loop
    for (const auto& eachLayer : mLayers) {
      eachLayer->onUpdate();
    }
    mSceneManager.updateScene();

    mRenderer.beginFrame();
    mRenderer.renderFrame();
    mRenderer.endFrame();
  }

  auto sceneUnloadResult = mSceneManager.unloadScene();
  if (!sceneUnloadResult) {
    LOG_ERROR("scene unload error: {}", default_msg(sceneUnloadResult.error().code));
    return make_error<void>(ErrorCode::EngineResourceGCFailed);
  }
  return {};
}

Result<void> Engine::attachLayer(std::unique_ptr<ILayer>&& layer) noexcept {
  PD_ASSERT_MSG(!mInitialized, "layer should be attached before initialize()");
  mLayers.push_back(std::move(layer));
  auto* pLayer = mLayers.back().get();
  auto result = pLayer->onAttached();
  PD_ASSERT(result);
  return {};
}

Result<std::unique_ptr<ILayer>> Engine::detachLayer(ILayer* layer) noexcept {
  auto findLayer = [layer](const auto& v) { return v.get() == layer; };
  auto layerIt = std::ranges::find_if(mLayers.begin(), mLayers.end(), findLayer);
  if (layerIt == mLayers.end()) {
    return make_error<std::unique_ptr<ILayer>>(ErrorCode::SceneLoadFailed);
  }
  auto returnLayer = std::move(*layerIt);
  auto result = returnLayer->onDetached();
  PD_ASSERT(result);
  mLayers.erase(layerIt);
  return returnLayer;
}
}  // namespace pd