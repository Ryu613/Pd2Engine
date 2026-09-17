#pragma once

#include "pd/backend/backend_types.hpp"

#include "pd/rendering/shader/shader_compiler.hpp"

namespace pd {
class ShaderManager {
 public:
  ShaderManager();
  ~ShaderManager();
  DELETE_COPY_MOVE(ShaderManager);

  Result<ShaderData> compile(ShaderSpec spec) noexcept;

 private:
  ShaderCompiler mCompiler;
  // todo: shader cache
};
}  // namespace pd