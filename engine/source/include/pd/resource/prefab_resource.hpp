#pragma once

#include "pd/resource/resource.hpp"
#include "pd/backend/backend.hpp"

namespace pd {
class PrefabResource : public Resource {
 public:
  ~PrefabResource() override;

 protected:
  Result<void> doLoad() noexcept override;
  Result<void> doUnload() noexcept override;

 private:
  friend class ResourceManager;

  Backend* mBackend = nullptr;

  explicit PrefabResource(ResourceIdType id, AssetIdType assetId, const std::string& name, Backend& backend);
};
}  // namespace pd