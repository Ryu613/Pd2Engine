#pragma once

#include "pd/platform/fs/file_system.hpp"

namespace pd {
class AssetManager {
 public:
  explicit AssetManager(IFileSystem* fs);
  ~AssetManager();
  DELETE_COPY_MOVE(AssetManager);

  Result<void> init() noexcept;
  Result<void> destroy() noexcept;

 private:
  IFileSystem* mFs = nullptr;
};
}  // namespace pd