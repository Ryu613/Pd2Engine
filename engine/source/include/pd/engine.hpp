#pragma once

#include "pd/core/globals.hpp"
#include "pd/core/allocators.hpp"
#include "pd/core/entity_manager.hpp"
#include "pd/platform/platform.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/rendering/renderer.hpp"
#include "pd/scene/scene_descriptor.hpp"
#include "pd/scene/scene.hpp"
#include "pd/scene/manager/transform_manager.hpp"
#include "pd/scene/manager/renderable_manager.hpp"
#include "pd/scene/manager/camera_manager.hpp"
#include "pd/scene/manager/light_manager.hpp"

namespace pd {
/**
 * 作为引擎全局入口，负责管理子系统的停启及监控运行状态，统一在此进行资源的创建和销毁
 */
class Engine {
 public:
  struct Config {
    std::string appName{"PdEngine"};
    uint32_t windowWidth = global::DEFAULT_WINDOW_WIDTH;
    uint32_t windowHeight = global::DEFAULT_WINDOW_HEIGHT;
    bool enableDebug = true;
  };
  enum class EngineError : uint8_t {
    SceneLoadError = 1,
    RunFailed = 2,
  };

  explicit Engine(Config& config) noexcept;
  ~Engine() noexcept;

  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
  Engine(Engine&&) noexcept = delete;
  Engine& operator=(Engine&&) noexcept = delete;

  bool initialize() noexcept;
  void shutdown() noexcept;

  template <DerivedSceneDescriptor T, typename... Args>
  std::expected<void, EngineError> loadScene(Args&&... args) noexcept {
    mSceneDescriptor = std::make_unique<T>(std::forward<Args>(args)...);
    mSceneDescriptor->onLoad(*mAssetManager, mEntityManager, mTransformManager,
                             mCameraManager, mScene);
  }

  std::expected<void, EngineError> run() noexcept;

 private:
  Config mConfig;
  bool mInitialized = false;

  HeapAllocator mArena;
  ResourceManager mResourceManager;
  EntityManager mEntityManager;
  // scene property managers
  TransformManager mTransformManager;
  RenderableManager mRenderableManager;
  CameraManager mCameraManager;
  LightManager mLightManager;

  std::unique_ptr<Platform> mPlatform;
  std::unique_ptr<AssetManager> mAssetManager;
  std::unique_ptr<Renderer> mRenderer;
  // 相当于world
  Scene mScene;

  std::unique_ptr<SceneDescriptor> mSceneDescriptor;
};
}  // namespace pd