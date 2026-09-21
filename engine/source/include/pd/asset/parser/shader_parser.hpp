#pragma once

#include "pd/asset/parser/asset_parser.hpp"

#include <filesystem>

namespace pd {
class IFileSystem;
class ShaderParser : public IAssetParser {
 public:
  explicit ShaderParser(IFileSystem* fs);
  ~ShaderParser() override;
  DEFAULT_MOVABLE(ShaderParser);
  DELETE_COPY(ShaderParser);

  Result<void> parse(Asset& asset) noexcept override;

 private:
  IFileSystem* mFs = nullptr;
  std::filesystem::path mBasePath;
};
}  // namespace pd