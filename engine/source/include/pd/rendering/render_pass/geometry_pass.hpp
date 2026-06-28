#pragma once

#include "pd/rendering/render_pass/render_pass.hpp"

namespace pd {
class GeometryPass : public RenderPass {
 public:
  GeometryPass(RenderGraph& rg);
  ~GeometryPass();
  void setup() noexcept override;
  void execute() noexcept override;
};
}  // namespace pd