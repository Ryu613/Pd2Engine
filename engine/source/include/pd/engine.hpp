#pragma once

#include "pd/core/globals.hpp"
#include "pd/core/allocators.hpp"
#include "pd/core/entity_manager.hpp"
#include "pd/platform/platform.hpp"
#include "pd/resource/resource_manager.hpp"
#include "pd/asset/asset_manager.hpp"
#include "pd/rendering/renderer.hpp"
#include "pd/scene/scene_description.hpp"
#include "pd/scene/scene.hpp"
#include "pd/scene/view.hpp"

namespace pd {
/**
 * 作为引擎全局入口，负责管理子系统的停启及监控运行状态，统一在此进行资源的创建和销毁
 * zi
 */
class Engine {
 public:
  struct Config {
    std::string appName{"PdEngine"};
    uint32_t windowWidth = global::DEFAULT_WINDOW_WIDTH;
    uint32_t windowHeight = global::DEFAULT_WINDOW_HEIGHT;
    bool enableDebug = true;
  };

  explicit Engine(Config& config) noexcept;
  ~Engine() noexcept;

  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
  Engine(Engine&&) noexcept = delete;
  Engine& operator=(Engine&&) noexcept = delete;

  bool initialize() noexcept;
  void shutdown() noexcept;

  template <DerivedSceneDescription T, typename... Args>
  void loadScene(Args&&... args) {
    T description{std::forward<Args>(args)...};
    description.preRender(mAssetManager);
  }

  void run();

 private:
  Config mConfig;
  bool mInitialized = false;

  HeapAllocator mArena;
  ResourceManager mResourceManager;
  EntityManager mEntityManager;

  std::unique_ptr<Platform> mPlatform;
  std::unique_ptr<AssetManager> mAssetManager;
  std::unique_ptr<Renderer> mRenderer;

  // TODO: 目前只支持一个场景+一个视窗
  Scene mScene;
  View mView;
};
}  // namespace pd