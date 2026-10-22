#pragma once

#include "pd/asset/compiler/decoder/texture_decoder.hpp"

namespace pd {
class Ktx2Decoder : public ITextureDecoder {
 public:
  Ktx2Decoder() = default;
  ~Ktx2Decoder() = default;
  DELETE_COPY_MOVE(Ktx2Decoder)

  Result<ImageData> decode(TextureData& texData) noexcept override;
};
}  // namespace pd