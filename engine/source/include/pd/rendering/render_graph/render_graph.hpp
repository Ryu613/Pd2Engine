#pragma once

#include "pd/rendering/render_pass/render_pass.hpp"

namespace pd {
template <typename T>
concept BaseOfRenderPass = std::derived_from<T, RenderPass>;
class RenderGraph {
 public:
  RenderGraph() noexcept = default;
  ~RenderGraph() = default;
  MOVABLE_ONLY(RenderGraph);

  template <BaseOfRenderPass Pass, typename... Args>
  void addPass(Args&&... args) noexcept {
    auto pass = std::make_unique<Pass>(std::forward<Args>(args)...);
    mRenderPasses.push_back(std::move(pass));
  }

  void reset() noexcept;

  void compile() noexcept;

  void execute() noexcept;

 private:
  std::vector<std::unique_ptr<RenderPass>> mRenderPasses;
  std::vector<PassNode> mNodes;
};

}  // namespace pd