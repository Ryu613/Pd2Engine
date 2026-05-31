#pragma once

namespace pd {
class Driver;
class View;
class Renderer {
 public:
  struct Config {};
  explicit Renderer(Driver& driver) noexcept;
  ~Renderer();

  NO_COPY_MOVE(Renderer);

  void beginFrame();
  void endFrame();
  void renderFrame(View& view);

 private:
  Driver& mDriver;
};
}  // namespace pd