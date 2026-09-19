#include "pd/rendering/shader/shader_manager.hpp"

namespace pd {
ShaderManager::ShaderManager() {}
ShaderManager::~ShaderManager() {}

Result<ShaderData> ShaderManager::compile(ShaderDesc spec) noexcept {
  auto compileRes = mCompiler.compile(spec);
  if (!compileRes) {
    LOG_ERROR("shader compile error: {}", compileRes.error().msg);
    return compileRes;
  }
  return compileRes;
}
}  // namespace pd