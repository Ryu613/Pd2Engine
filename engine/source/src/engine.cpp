#include "pd/engine.hpp"

#include "pd/platform/platform_factory.hpp"
#include "pd/backend/backend_factory.hpp"

namespace pd {

Engine::Engine(Config config) noexcept
    : mConfig{std::move(config)},
      mArena{"Engine Heap Allocator", ResourceType::NullResource{}},
      mRenderer(Renderer::Config{}) {
  log::info("Engine created!");
  mLayers.reserve(4);
  log::logo();
}

Engine::~Engine() noexcept { shutdown(); }

Result<void, Error::Engine> Engine::initialize() noexcept {
  // 初始化平台层
  IPlatform::Config config{
      .window =
          {
              .title = mConfig.appName,
              .width = mConfig.windowWidth,
              .height = mConfig.windowHeight,
          },
  };
  mPlatform = createPlatform(std::move(config));

  // 创建窗口
  if (!mPlatform->window()->create()) {
    log::error("window creation failed!");
    return std::unexpected<Error::Engine>(Error::Engine::InitializeFailed);
  }
  // 初始化渲染后端
  auto* pWindow = mPlatform->window();
  IBackend::Config backendConfig{
      .pWindow = pWindow,
  };
  mBackend = createBackend(backendConfig);

  // 初始化其他子系统
  mResourceManager.initialize(mBackend.get());
  mRenderer.initialize(mBackend.get(), pWindow);
  mSceneManager.initialize(&mAssetManager, &mResourceManager);

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
      log::error("layer detach failed! layer: {}, error: {}", layer->name(),
                 result.error());
      continue;
    }
  }

  mInitialized = false;
}

Result<void, Error::Engine> Engine::run() noexcept {
  if (!mInitialized) {
    return std::unexpected<Error::Engine>(Error::Engine::RunFailed);
  }
  // prepare scene data
  auto sceneLoadResult = mSceneManager.loadScene();
  if (!sceneLoadResult) {
    log::error("scene load failed!, error: {}", sceneLoadResult.error());
    return std::unexpected<Error::Engine>(Error::Engine::RunFailed);
  }
  while (!mPlatform->window()->shouldClose()) {
    mPlatform->processEvents();
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
    log::error("scene unload error: {}", sceneUnloadResult.error());
    return std::unexpected<Error::Engine>(Error::Engine::ResourceGCFailed);
  }
  return {};
}

Result<ILayer*, Error::Layer> Engine::attachLayer(
    std::unique_ptr<ILayer>&& layer) noexcept {
  PD_ASSERT_MSG(!mInitialized, "layer should be attached before initialize()");
  mLayers.push_back(std::move(layer));
  auto* pLayer = mLayers.back().get();
  auto result = pLayer->onAttached();
  PD_ASSERT(result);
  return {};
}

Result<std::unique_ptr<ILayer>, Error::Layer> Engine::detachLayer(
    ILayer* layer) noexcept {
  auto findLayer = [layer](const auto& v) { return v.get() == layer; };
  auto layerIt = std::ranges::find_if(mLayers.begin(), mLayers.end(), findLayer);
  if (layerIt == mLayers.end()) {
    return std::unexpected(Error::Layer::LayerNotFound);
  }
  auto returnLayer = std::move(*layerIt);
  auto result = returnLayer->onDetached();
  PD_ASSERT(result);
  mLayers.erase(layerIt);
  return returnLayer;
}
}  // namespace pd