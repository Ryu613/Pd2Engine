#pragma once

#include <span>

#include "pd/resource/resource.hpp"
#include "pd/backend/backend.hpp"

namespace pd {
class ShaderResource : public Resource {
 public:
  ~ShaderResource() override;
  DELETE_COPY(ShaderResource);
  DEFAULT_MOVABLE(ShaderResource);

 protected:
  Result<void> doLoad() noexcept override;
  Result<void> doUnload() noexcept override;

 private:
  friend class ResourceManager;

  std::span<const std::byte> mSource;
  Backend* mBackend = nullptr;
  HwShaderModuleHandle mShaderModule;

  explicit ShaderResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend);

  void setSource(std::span<const std::byte> shaderCode) noexcept { mSource = shaderCode; }
  void clearSource() noexcept { mSource = {}; }
};
}  // namespace pd