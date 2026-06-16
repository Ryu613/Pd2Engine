#pragma once

namespace pd {
class VulkanContext;
class VulkanResourceManager {
 public:
  VulkanResourceManager() noexcept = default;
  explicit VulkanResourceManager(VulkanContext* ctx) noexcept;
  ~VulkanResourceManager() = default;
  MOVABLE_ONLY(VulkanResourceManager);

 private:
  VulkanContext* mVulkanContext = nullptr;
};
}  // namespace pd