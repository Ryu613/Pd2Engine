#pragma once

#include "pd/asset/compiler/decoder/texture_decoder.hpp"

namespace pd {
class Ktx2Decoder : public ITextureDecoder {
 public:
  Ktx2Decoder();
  ~Ktx2Decoder();
  DELETE_COPY(Ktx2Decoder);
  DEFAULT_MOVABLE(Ktx2Decoder);

  Result<std::vector<const std::byte>> decode(TextureData& texData) noexcept override;
};
}  // namespace pd