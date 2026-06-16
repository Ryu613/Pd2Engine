#pragma once

namespace pd {
class Backend;
class View;
class Renderer {
 public:
  struct Config {};
  Renderer() noexcept = default;
  ~Renderer();

  MOVABLE_ONLY(Renderer);

  void beginFrame();
  void renderFrame(View& view);
  void endFrame();

 private:
  Backend* mBackend;
};
}  // namespace pd