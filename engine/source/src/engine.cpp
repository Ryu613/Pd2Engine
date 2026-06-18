#include "pd/engine.hpp"

#include "pd/platform/platform_factory.hpp"
#include "pd/backend/backend_factory.hpp"

namespace pd {
using Error = Engine::Error;

Engine::Engine(Config config) noexcept
    : mConfig{std::move(config)},
      mArena{"Engine Heap Allocator", ResourceType::NullResource{}} {
  log::info("Engine created!");
  mLayers.reserve(4);
  log::logo();
}

Engine::~Engine() noexcept { shutdown(); }

Engine::EngineResult<void> Engine::initialize() noexcept {
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

  // 初始化渲染后端
  if (!mPlatform->window()->create()) {
    log::error("window creation failed!");
    return std::unexpected<Error>(Error::InitializeFailed);
  }
  // 初始化渲染后端
  auto* pWindow = mPlatform->window();
  IBackend::Config backendConfig{
      .pWindow = pWindow,
  };
  mBackend = createBackend(backendConfig);
  //   if (!mPlatform->initialize(mConfig.appName, mConfig.windowWidth,
  //   mConfig.windowHeight,
  //                              mConfig.enableDebug)) {
  //     log::error("platform creation failed!");
  //     return std::unexpected<EngineError>(EngineError::InitializeFailed);
  //   }
  //   mAssetManager = std::make_unique<AssetManager>(mPlatform->getFileSystem(),
  //                                                  mEntityManager, mResourceManager);

  // 初始化 engine layer
  //   auto engineLayer = std::make_unique<EngineLayer>(mPlatform->window());

  //   auto result = attachLayer(std::move(engineLayer));
  //   if (!result) {
  //     log::error("engine layer onAttach() failed!");
  //     return std::unexpected<Error>(Error::InitializeFailed);
  //   }

  // 创建交换链
  //   mBackend.createSwapchain(pWindow->windowWidth(), pWindow->windowHeight());

  mInitialized = true;
  return {};
}

void Engine::shutdown() noexcept {
  if (!mInitialized) {
    return;
  }
  // explicit destroy subsystem
  //   mRenderer.reset();
  //   mAssetManager.reset();
  mPlatform.reset();

  mInitialized = false;
}

Engine::EngineResult<void> Engine::run() noexcept {
  if (!mInitialized) {
    return std::unexpected<Error>(Error::RunFailed);
  }
  while (!mPlatform->window()->shouldClose()) {
    //     mPlatform->processEvents();
    for (const auto& eachLayer : mLayers) {
      eachLayer->onUpdate();
    }
    //     mRenderer->beginFrame();
    //     // mRenderer->renderFrame();
    //     mRenderer->endFrame();
  }
  return {};
}

Engine::EngineResult<ILayer*> Engine::attachLayer(
    std::unique_ptr<ILayer>&& layer) noexcept {
  mLayers.push_back(std::move(layer));
  auto* pLayer = mLayers.back().get();
  auto result = pLayer->onAttached();
  PD_ASSERT(result);
  return {};
}

Engine::EngineResult<std::unique_ptr<ILayer>> Engine::detachLayer(
    ILayer* layer) noexcept {
  auto findLayer = [layer](const auto& v) { return v.get() == layer; };
  auto layerIt = std::ranges::find_if(mLayers.begin(), mLayers.end(), findLayer);
  if (layerIt == mLayers.end()) {
    return std::unexpected(Error::LayerNotFound);
  }
  auto returnLayer = std::move(*layerIt);
  auto result = returnLayer->onDetached();
  PD_ASSERT(result);
  mLayers.erase(layerIt);
  return returnLayer;
}
}  // namespace pd