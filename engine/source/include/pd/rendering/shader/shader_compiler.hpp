#pragma once

#include "pd/backend/backend_types.hpp"

namespace pd {
class ShaderCompiler {
 public:
  ShaderCompiler();
  ~ShaderCompiler();
  DELETE_COPY_MOVE(ShaderCompiler);

  Result<ShaderData> compile(const ShaderSpec& spec) noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd