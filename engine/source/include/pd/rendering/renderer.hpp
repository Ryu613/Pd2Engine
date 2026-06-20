#pragma once

#include "pd/backend/hw_handle.hpp"
#include "pd/backend/hw_swapchain.hpp"

namespace pd {
class IWindow;
class IBackend;
class View;
class Renderer {
 public:
  struct Config {};
  Renderer() noexcept = default;
  explicit Renderer(IBackend* pBackend, IWindow* pWindow) noexcept;
  ~Renderer();

  MOVABLE_ONLY(Renderer);

  void beginFrame();
  void renderFrame(View& view);
  void endFrame();

 private:
  IBackend* mBackend = nullptr;
  IWindow* mWindow = nullptr;
  HwHandle<Swapchain_t> mSwapchain;
  bool mSwapchainDirty = false;

  void init() noexcept;
  void destroy() noexcept;
};
}  // namespace pd