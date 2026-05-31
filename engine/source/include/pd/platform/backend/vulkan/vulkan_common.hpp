#pragma once

#define VK_CHECK(x)                                                               \
  do {                                                                            \
    VkResult err = x;                                                             \
    if (err) {                                                                    \
      log::error(fmt::format("Detected Vulkan error: {}", string_VkResult(err))); \
      return false;                                                               \
    }                                                                             \
  } while (0)

namespace pd {
struct VulkanConfig {
  std::string appName;
  bool enableValidationLayer = true;
  uint32_t windowWidth = global::DEFAULT_WINDOW_WIDTH;
  uint32_t windowHeight = global::DEFAULT_WINDOW_HEIGHT;
  void* nativeWindowHandle = nullptr;
  std::vector<const char*> requiredInstanceExtensions;
};

static constexpr uint32_t INVALID_VK_INDEX = UINT32_MAX;
static constexpr size_t INFLIGHT_FRAME_COUNT = 2;
}  // namespace pd