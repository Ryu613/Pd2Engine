#include "pd/asset/compiler/texture_compiler.hpp"

#include "pd/asset/compiler/decoder/stb_decoder.hpp"

namespace pd {
TextureCompiler::TextureCompiler() { initDecoders(); }
TextureCompiler::~TextureCompiler() {}

Result<ImageData> TextureCompiler::compile(TextureData& tex) noexcept {
  return mDecoders[tex.decoderType]->decode(tex);
}

void TextureCompiler::initDecoders() noexcept {
  mDecoders.emplace(TextureDecoderType::Stb, std::make_unique<StbDecoder>());
}
}  // namespace pd