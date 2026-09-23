#pragma once

#include <span>

namespace pd {
/**
 * @brief 着色器编译器不需要运行时多态，在构建时确定着色器语言
 */
struct ShaderDesc {
  std::string moduleName;
  std::string modulePath;
  std::span<const std::byte> code;
};
struct ShaderData {
  std::vector<u8> spirvCode;
  // todo: reflection info
};
class ShaderCompiler {
 public:
  ShaderCompiler();
  ~ShaderCompiler();
  DELETE_COPY(ShaderCompiler);
  DEFAULT_MOVABLE(ShaderCompiler);

  Result<ShaderData> compile(const ShaderDesc& spec) noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};
}  // namespace pd