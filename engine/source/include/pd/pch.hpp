#pragma once
// std
#include <memory>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <string>
#include <cstdint>

// 3rd-party
#include "magic_enum/magic_enum.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

// globals
#include "pd/core/globals.hpp"
#include "pd/core/assert.hpp"
#include "pd/core/logging.hpp"
