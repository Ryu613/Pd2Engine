#pragma once

#include "SDL3/sdl.h"

#ifdef BACKEND_VULKAN
#include "SDL3/SDL_vulkan.h"
#endif

namespace pd {
class SDL3Helper {
 public:
  static void* getNativeWindowHandle(SDL_Window* window) noexcept {
    if (window == nullptr) {
      return nullptr;
    }
    return SDL_GetPointerProperty(SDL_GetWindowProperties(window),
                                  SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
  }

  static std::vector<const char*> getVulkanInstanceExtensions() noexcept {
    uint32_t instanceExtensionsCount{0};
#ifdef BACKEND_VULKAN
    const char* const* instanceExtensions{
        SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount)};
    return std::vector<const char*>(instanceExtensions,
                                    instanceExtensions + instanceExtensionsCount);
#else
    PD_ASSERT_MSG(false, "illegal operation!");
#endif
  }
};
}  // namespace pd