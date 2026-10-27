#include "pd/resource/shader_resource.hpp"

namespace pd {
ShaderResource::ShaderResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend)
    : Resource(id, assetId, name),
      mBackend(&backend) {}

ShaderResource::~ShaderResource() {}

Result<void> ShaderResource::doLoad() noexcept {
  LOG_INFO("loading shader: id={}, name={}", this->id(), this->name());
  ShaderModuleCreateDesc desc{
      .spirCode = mSource,
  };
  auto shaderHandle = mBackend->createShaderModule(desc);
  PD_ASSERT(shaderHandle);

  mShaderModule = shaderHandle;
  return {};
}
Result<void> ShaderResource::doUnload() noexcept {
  if (!mShaderModule) {
    return {};
  }
  mBackend->destroyShaderModule(mShaderModule);
  return {};
}
}  // namespace pd