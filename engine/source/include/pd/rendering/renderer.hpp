#pragma once

#include "pd/scene/scene_manager.hpp"
#include "pd/backend/backend.hpp"

namespace pd {
class Renderer {
 public:
  explicit Renderer(SceneManager* sceneMgr, Backend* backend);
  ~Renderer();
  DELETE_COPY_MOVE(Renderer);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

  void renderFrame() noexcept;

 private:
  struct FrameContext {
    FrameData data;
    CommandRecorder cmdRecorder;
  };

  SceneManager* mSceneManager = nullptr;
  Backend* mBackend = nullptr;

  FrameContext beginFrame() noexcept;
  void doFrame(FrameContext& data) noexcept;
  void endFrame(FrameContext& data) noexcept;
};
}  // namespace pd