#include "pd/rendering/render_graph/render_graph.hpp"

namespace pd {

void RenderGraph::reset() noexcept {
  mRenderPasses.clear();
  mNodes.clear();
}

void RenderGraph::setup() noexcept {
  mNodes.clear();
  // shortcut: simplefield for testing
  // TODO(author): node culling, resources and passes dependencies analysis, build dag
  PassNode currentNode{
      .id = 0,
      .pPass = mRenderPasses[0].get(),
  };
  mNodes.push_back(currentNode);
  for (size_t i = 1; i < mRenderPasses.size(); ++i) {
    auto tailIt = mNodes.end();
    PassNode node{
      .id = static_cast<uint32_t>(i),
      .pPass = mRenderPasses[i].get(),
    };
    tailIt->pNext = node.pPass;
    mNodes.push_back(node);
  }
}

void RenderGraph::compile() noexcept {
  // TODO(author): dag's nodes resources allocation
  for (auto& node : mNodes) {
    node.pPass->setup();
  }
}

void RenderGraph::execute() noexcept {
  // TODO(author)
  for (auto& node : mNodes) {
    node.pPass->execute();
  }
}
}  // namespace pd