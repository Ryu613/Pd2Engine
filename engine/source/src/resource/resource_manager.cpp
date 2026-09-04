#include "pd/resource/resource_manager.hpp"

namespace pd {
ResourceManager::ResourceManager(Backend* backend)
    : mBackend(backend) {}
    
ResourceManager::~ResourceManager() {}
}  // namespace pd