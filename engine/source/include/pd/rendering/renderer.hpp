#pragma once

namespace pd {
class RhiApi;
class View;
class Renderer {
 public:
  struct Config {};
  explicit Renderer(RhiApi& rhiApi) noexcept;
  ~Renderer();

  NO_COPY_MOVE(Renderer);

  void beginFrame();
  void endFrame();
  void renderFrame(View& view);

 private:
  RhiApi& mRhiApi;
};
}  // namespace pd