#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"
#include "pd/rendering/render_graph/render_graph.hpp"
#include "pd/scene/scene_manager.hpp"
#include "pd/resource/resource_manager.hpp"

namespace pd {
class IWindow;
class IBackend;
class View;
class Scene;
struct FrameData {};
struct FrameContext {};
class Renderer {
 public:
  enum class InFlightFrame : u8 {
    Double = 2,
    Triple = 3,
  };
  enum class RenderPipeline : u8 {
    Forward,
    Deferred,
    PathTracing,
  };
  enum class LightCulling : u8 {
    None,
    Tiled,
    Clustered,
  };
  enum class ShadingModel : u8 {
    Unlit,
    BlinnPhong,
    StandardPBR,
    SpectralPBR,
    Toon,
    Neural,

  };

  struct Config {
    RenderPipeline renderPipeline = RenderPipeline::Forward;
    LightCulling lightCulling = Renderer::LightCulling::None;
    ShadingModel shadingModel = Renderer::ShadingModel::StandardPBR;
    InFlightFrame inFlightFrame = InFlightFrame::Double;
  };

  explicit Renderer(Config config) noexcept;
  ~Renderer();
  DELETE_COPY_MOVE(Renderer);

  void initialize(IBackend* pBackend, IWindow* pWindow, SceneManager* pSceneManager,
                  ResourceManager* pResourceManager) noexcept;

  void destroy() noexcept;

  void beginFrame();
  void renderFrame();
  void endFrame();

 private:
  IBackend* mBackend = nullptr;
  IWindow* mWindow = nullptr;
  SceneManager* mSceneManager = nullptr;
  ResourceManager* mResourceManager = nullptr;

  Config mConfig;
  RenderGraph mRenderGraph;
  HwHandle<Swapchain_t> mSwapchain;
  FrameContext mFrameContext;
  u32 frameIndex = 0;
  std::vector<FrameData> mFrames;
  bool mSwapchainDirty = false;
  bool mInitialized = false;

  void initializeRenderGraph() noexcept;
  void render(Renderable& renderable) noexcept;
};
}  // namespace pd