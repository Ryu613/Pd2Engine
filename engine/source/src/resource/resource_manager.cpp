#include "pd/resource/resource_manager.hpp"

namespace pd {
ResourceManager::ResourceManager(Backend* backend)
    : mBackend(backend) {}

ResourceManager::~ResourceManager() {}

Result<void> ResourceManager::init() noexcept { return {}; }

Result<void> ResourceManager::destroy() noexcept { return {}; }
}  // namespace pd