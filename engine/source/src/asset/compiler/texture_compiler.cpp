#include "pd/asset/compiler/texture_compiler.hpp"

namespace pd {
TextureCompiler::TextureCompiler() { initDecoders(); }
TextureCompiler::~TextureCompiler() {}

Result<void> TextureCompiler::compile(TextureData& tex) noexcept { return {}; }

void TextureCompiler::initDecoders() noexcept {}
}  // namespace pd