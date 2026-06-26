#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"
#include "pd/rendering/render_graph/render_graph.hpp"

namespace pd {
class IWindow;
class IBackend;
class View;
class Renderer {
 public:
  enum class ShadingPath : uint8_t {
    Forward,
    ForwardPlus,
    Deferred,
    ClusterDeferred,
  };

  struct Config {
    ShadingPath shadingPath = ShadingPath::Forward;
  };

  explicit Renderer(Config config) noexcept;
  ~Renderer();
  MOVABLE_ONLY(Renderer);

  void initialize(IBackend* pBackend, IWindow* pWindow) noexcept;

  void destroy() noexcept;

  void beginFrame();
  void renderFrame();
  void endFrame();

 private:
  IBackend* mBackend = nullptr;
  IWindow* mWindow = nullptr;

  Config mConfig;
  RenderGraph mRenderGraph;
  HwHandle<Swapchain_t> mSwapchain;
  bool mSwapchainDirty = false;
  bool mInitialized = false;

  void initializeRenderGraph() noexcept;
};
}  // namespace pd