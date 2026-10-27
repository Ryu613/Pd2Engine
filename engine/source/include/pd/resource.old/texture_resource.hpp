#pragma once

#include <span>

#include "pd/resource/resource.hpp"
#include "pd/backend/backend.hpp"

namespace pd {
class TextureResource : public Resource {
 public:
  struct Desc {};

  ~TextureResource();
  DELETE_COPY(TextureResource);
  DEFAULT_MOVABLE(TextureResource);

 protected:
  Result<void> doLoad() noexcept override;
  Result<void> doUnload() noexcept override;

 private:
  friend class ResourceManager;

  std::span<uint8_t> mRawData;
  HwTextureHandle mTexture;

  explicit TextureResource(ResourceIdType id, const std::string& name, const Desc info, std::span<uint8_t> rawData);
};
}  // namespace pd