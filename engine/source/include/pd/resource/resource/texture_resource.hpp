#pragma once

#include "pd/resource/resource.hpp"
#include "pd/asset/asset_types.hpp"

namespace pd {
class TextureResource : public Resource {
 public:
  struct Info {};

  ~TextureResource() = default;
  DELETE_COPY(TextureResource);
  DEFAULT_MOVABLE(TextureResource);

 protected:
  Result<void> doLoad(LoadContext& ctx) noexcept override;
  Result<void> doUnload(LoadContext& ctx) noexcept override;

 private:
  friend class ResourceManager;

  Info mInfo;
  HwTextureHandle mTexture;

  explicit TextureResource(u64 sourceId, ResourceVariantIdType variantId = defaultVariantId,
                           std::string name = "texture")
      : Resource(sourceId, variantId, std::move(name)) {}
};
}  // namespace pd