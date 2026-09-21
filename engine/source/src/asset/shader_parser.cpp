#include "pd/asset/parser/shader_parser.hpp"

#include "pd/asset/asset.hpp"
#include "pd/platform/fs/file_system.hpp"

namespace pd {
ShaderParser::ShaderParser(IFileSystem* fs)
    : mFs(fs) {}
ShaderParser::~ShaderParser() {}

Result<void> ShaderParser::parse(Asset& asset) noexcept {
  auto& shaderAsset = static_cast<ShaderAsset&>(asset);
  auto code = mFs->readFileBinary(shaderAsset.info().path);
  if (code.empty()) {
    return make_error<void>(ErrorCode::AssetFileLoadError);
  }
  shaderAsset.mCode = code;
  return {};
}
}  // namespace pd