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
  ~Renderer();

  void initialize(IBackend* pBackend, IWindow* pWindow) noexcept;

  void destroy() noexcept;

  MOVABLE_ONLY(Renderer);

  void beginFrame();
  void renderFrame(View& view);
  void endFrame();

 private:
  IBackend* mBackend = nullptr;
  IWindow* mWindow = nullptr;
  HwHandle<Swapchain_t> mSwapchain;
  bool mSwapchainDirty = false;
  bool mInitialized = false;
};
}  // namespace pd