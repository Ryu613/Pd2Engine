#pragma once

#include "pd/backend/backend.hpp"
#include "pd/platform/window/window.hpp"

#if defined(BACKEND_VULKAN)
#include "pd/backend/vulkan/backend_vulkan.hpp"
#else
#error "BACKEND NOT SUPPORTED!"
#endif

namespace pd {
template <typename... Args>
std::unique_ptr<IBackend> createBackend(IBackend::Config config,
                                        [[maybe_unused]] Args&&... args) {
#ifdef BACKEND_VULKAN
  VulkanContext::Config vulkanConfig{
      .appName = config.pWindow->windowTitle(),
      .nativeWindowHandle = config.pWindow->nativeHandle(),
      .requiredInstanceExtensions = config.pWindow->getVulkanInstanceExtensions()};
  auto context = std::make_unique<VulkanContext>(vulkanConfig);
  return std::make_unique<BackendVulkan>(std::move(context));
#else
  PD_ASSERT_MSG(false, "Backend Error!");
#endif  // _WIN32
}
}  // namespace pd