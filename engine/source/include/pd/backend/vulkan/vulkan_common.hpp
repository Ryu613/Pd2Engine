#pragma once

#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR 1
#else
#error "unsupported system!"
#endif
#define VK_NO_PROTOTYPES
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.h"
#include "volk.h"
#include "vulkan/vk_enum_string_helper.h"

#define VK_CHECK(x)                                                               \
  do {                                                                            \
    VkResult err = x;                                                             \
    if (err) {                                                                    \
      log::error(fmt::format("Detected Vulkan error: {}", string_VkResult(err))); \
      PD_ASSERT_MSG(false, "vulkan operation error!");                            \
    }                                                                             \
  } while (0)

namespace pd {
namespace global {

inline constexpr size_t InFlightFrameCount = 2;
inline constexpr uint32_t VkInvalidIndex = UINT32_MAX;

}  // namespace global
}  // namespace pd