#include "pd/rendering/shader/shader_compiler.hpp"

namespace pd {
class ShaderCompiler::Impl {
 public:
  Result<std::vector<u8>> compile(const std::vector<u8>& code) noexcept { return {}; };
};

ShaderCompiler::ShaderCompiler() {}
ShaderCompiler::~ShaderCompiler() {}

Result<std::vector<u8>> ShaderCompiler::compile(const std::vector<u8>& code) noexcept { return mImpl->compile(code); }
}  // namespace pd