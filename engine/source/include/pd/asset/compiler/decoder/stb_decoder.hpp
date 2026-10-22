#pragma once

#include "pd/asset/compiler/decoder/texture_decoder.hpp"

namespace pd {
class StbDecoder : public ITextureDecoder {
 public:
  StbDecoder() = default;
  ~StbDecoder() = default;
  DELETE_COPY_MOVE(StbDecoder);

  Result<ImageData> decode(TextureData& texData) noexcept override;
};
}  // namespace pd