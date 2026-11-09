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

  void updateFrameConstants(const FrameConstantsDesc& frameConstantsDesc) noexcept;

  void endFrame(CommandRecorder cmdRecorder) noexcept;

  void waitIdle() noexcept;

  // resources

  // create
  HwShaderModuleHandle createShaderModule(const ShaderModuleCreateDesc& shaderModuleCreateDesc) noexcept;
  HwPipelineLayoutHandle createPipelineLayout(const PipelineLayoutCreateDesc& pipelineLayoutDesc) noexcept;
  HwGraphicsPipelineHandle createGraphicsPipeline(const GraphicsPipelineCreateDesc& pipelineDesc) noexcept;

  HwBufferHandle createBuffer(const BufferCreateDesc& bufferCreateDesc) noexcept;
  HwTextureHandle createTexture(const TextureCreateDesc& textureCreateDesc) noexcept;

  // destroy
  void destroyBuffer(HwBufferHandle handle) noexcept;
  void destroyPipelineLayout(HwPipelineLayoutHandle handle) noexcept;
  void destroyGraphicsPipeline(HwGraphicsPipelineHandle handle) noexcept;
  void destroyShaderModule(HwShaderModuleHandle handle) noexcept;
  void destroyTexture(HwTextureHandle handle) noexcept;

  void writeBuffer(const BufferWriteDesc& bufferWriteDesc) noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd