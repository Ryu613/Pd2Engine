#pragma once

namespace pd {
class TextureData;
// TODO
class ITextureCompiler {
 public:
  ITextureCompiler() = default;
  virtual ~ITextureCompiler() = default;
  DELETE_COPY(ITextureCompiler);
  DEFAULT_MOVABLE(ITextureCompiler);

  virtual Result<void> compile(TextureData& tex) noexcept { return {}; }
};
}  // namespace pd