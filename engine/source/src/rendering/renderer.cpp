#include "pd/rendering/renderer.hpp"

#include "pd/platform/rhi/rhi_api.hpp"

namespace pd {
Renderer::Renderer(RhiApi& rhiApi) noexcept
    : mRhiApi(rhiApi) {}

Renderer::~Renderer() {}

void Renderer::beginFrame() {
  // 让driver执行完上帧后获取到新的swapchain image
  // TODO(author): 目前Driver(RHI)不实现，先直接下转
//   auto& driver = static_cast<VulkanDriver&>(mDriver);
//   driver.acquireNextFrame();
  // 指令重置及开始录制
//   driver.startCmdRecording();
}

void Renderer::endFrame() {
//   auto& driver = static_cast<VulkanDriver&>(mDriver);
  // 停止指令录制
//   driver.endCmdRecording();
  // 提交指令
//   driver.submitFrame();
  // 呈现
//   driver.presentFrame();
  // 结束当前帧
//   driver.endFrame();
}

void Renderer::renderFrame(View& view) {
  // prepare in-frame view & scene data(using job system)
  // view.prepare();
  // build render graph
  // compile render graph(build dependency graph)
  // execute render graph(record all rendering commands)
}
}  // namespace pd