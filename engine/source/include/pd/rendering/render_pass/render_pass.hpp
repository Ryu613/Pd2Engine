#pragma once

namespace pd {

class RenderPass {
 public:
  RenderPass() noexcept = default;
  virtual ~RenderPass() = default;
  MOVABLE_ONLY(RenderPass);

  virtual void setup() noexcept = 0;
  virtual void execute() noexcept = 0;
};

struct PassNode {
  RenderPass* pPass = nullptr;
};
}  // namespace pd