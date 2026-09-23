#include "pd/asset/parser/shader_parser.hpp"

#include "pd/asset/asset.hpp"
#include "pd/platform/fs/file_system.hpp"
#include "pd/asset/compiler/shader_compiler.hpp"

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
  ShaderDesc desc{
      .moduleName = asset.info().shaderInfo.moduleName,
      .modulePath = asset.info().shaderInfo.modulePath,
      .code = std::as_bytes(std::span{code}),
  };
  auto compileRes = mCompiler.compile(desc);
  if (!compileRes) {
    LOG_ERROR("shader compile failed: {}", compileRes.error().msg);
    return make_error<void>(compileRes.error().code);
  }
  auto& spirvCode = compileRes.value().spirvCode;

  shaderAsset.setParsedCode(spirvCode);
 
  shaderAsset.mReflectionInfo = {
      // todo
  };
  return {};
}
}  // namespace pd