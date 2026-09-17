#pragma once

namespace pd {
class ShaderCompiler {
 public:
  ShaderCompiler();
  ~ShaderCompiler();
  DELETE_COPY_MOVE(ShaderCompiler);

  Result<std::vector<u8>> compile(const std::vector<u8>& code) noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd