#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"
#include "pd/rendering/render_graph/render_graph.hpp"

namespace pd {
class IWindow;
class IBackend;
class View;
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
  NO_COPY_MOVE(Renderer);

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
  std::vector<FrameContext> mFrames;
  bool mSwapchainDirty = false;
  bool mInitialized = false;

  void initializeRenderGraph() noexcept;
};
}  // namespace pd