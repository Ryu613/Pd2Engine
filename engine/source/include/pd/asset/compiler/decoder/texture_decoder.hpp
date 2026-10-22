#pragma once

#include "pd/asset/asset_types.hpp"

namespace pd {
class ITextureDecoder {
 public:
  ITextureDecoder() = default;
  virtual ~ITextureDecoder() = default;
  DELETE_COPY_MOVE(ITextureDecoder);

  virtual Result<ImageData> decode(TextureData& texData) noexcept = 0;
};
}  // namespace pd