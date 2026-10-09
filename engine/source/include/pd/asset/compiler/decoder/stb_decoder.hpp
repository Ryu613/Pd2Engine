#pragma once

#include "pd/asset/compiler/decoder/texture_decoder.hpp"

namespace pd {
class StbDecoder : public ITextureDecoder {
 public:
  StbDecoder();
  ~StbDecoder();
  DELETE_COPY(StbDecoder);
  DEFAULT_MOVABLE(StbDecoder);

  Result<std::vector<const std::byte>> decode(TextureData& texData) noexcept override;
};
}  // namespace pd