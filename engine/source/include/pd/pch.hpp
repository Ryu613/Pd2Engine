#pragma once
// std
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <array>
#include <type_traits>
#include <algorithm>
#include <cstdint>

// 3rd-party
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

// vulkan
#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR 1
#else
#error "unsupported system!"
#endif
#define VK_NO_PROTOTYPES
#include "vk_mem_alloc.h"
#include "volk.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.h"

// globals
#include "pd/core/globals.hpp"
#include "pd/core/assert.hpp"
#include "pd/core/logging.hpp"
