#pragma once

#include "pd/rendering/render_pass/render_pass.hpp"

namespace pd {
class ForwardPass : public RenderPass {
 public:
  void setup() noexcept override;
  void execute() noexcept override;
};
}  // namespace pd