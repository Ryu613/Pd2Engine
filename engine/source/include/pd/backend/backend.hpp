#pragma once

#include "pd/backend/command_recorder.hpp"
#include "pd/backend/backend_types.hpp"

namespace pd {

class Backend {
 public:
  Backend();
  ~Backend();
  DELETE_COPY_MOVE(Backend);

  Result<void> init(const BackendConfig& config) noexcept;
  Result<void> destroy() noexcept;

  [[nodiscard]] BackendApi backendApi() const noexcept;

  FrameData beginFrame() noexcept;

  void endFrame(CommandRecorder cmdRecorder) noexcept;

  void waitIdle() noexcept;

  // resources
  PipelineData createGraphicsPipeline(const GraphicsPipelineCreateDesc& desc) noexcept;
  void destroyGraphicsPipeline(const GraphicsPipelineDestroyDesc& desc) noexcept;

  HwBufferHandle createBuffer(const BufferCreateDesc& bufferCreateDesc) noexcept;
  void writeBuffer(const BufferWriteDesc& bufferWriteDesc) noexcept;
  void destroyBuffer(HwBufferHandle handle) noexcept;

  HwShaderModuleHandle createShaderModule(const ShaderModuleCreateDesc& shaderModuleCreateDesc) noexcept;
  void destroyShaderModule(HwShaderModuleHandle handle) noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd