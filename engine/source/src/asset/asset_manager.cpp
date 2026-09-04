#include "pd/asset/asset_manager.hpp"

namespace pd {
AssetManager::AssetManager(IFileSystem* fs)
    : mFs(fs) {}

AssetManager::~AssetManager() {}

Result<void> AssetManager::init() noexcept { return {}; }
Result<void> AssetManager::destroy() noexcept { return {}; }
}  // namespace pd