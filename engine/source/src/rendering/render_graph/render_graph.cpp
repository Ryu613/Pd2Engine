#include "pd/rendering/render_graph/render_graph.hpp"

namespace pd {

void RenderGraph::reset() noexcept {
  mRenderPasses.clear();
  mNodes.clear();
}

void RenderGraph::compile() noexcept {
  // TODO(author)
  mNodes.clear();
  for (auto& pass : mRenderPasses) {
    mNodes.emplace_back(pass.get());
  }
}

void RenderGraph::execute() noexcept {
  // TODO(author)
  for (auto& node : mNodes) {
    node.pPass->execute();
  }
}
}  // namespace pd