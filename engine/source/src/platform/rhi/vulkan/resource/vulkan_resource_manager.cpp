#include "pd/platform/rhi/vulkan/resource/vulkan_resource_manager.hpp"

namespace pd {
    RhiHandle<RhiTexture> VulkanResourceManager::createTexture(uint32_t width, uint32_t height,
                                      uint32_t depth) noexcept {
                                        RhiHandle<RhiTexture> handle{generateHandleId()};
                                        return handle;
                                      }

    uint32_t VulkanResourceManager::generateHandleId() noexcept {
        return mCurrentId++;
    }
}