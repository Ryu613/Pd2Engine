#pragma once

namespace pd {
class Backend;
class View;
class Renderer {
 public:
  struct Config {};
  Renderer() noexcept = default;
  ~Renderer();

  NO_COPY_MOVE(Renderer);

  void beginFrame();
  void renderFrame(View& view);
  void endFrame();

 private:
  Backend* mBackend;
};
}  // namespace pd