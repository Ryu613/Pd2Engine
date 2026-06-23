#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

namespace pd {
namespace math {
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using mat4 = glm::mat4;
using quat = glm::quat;

}  // namespace math
}  // namespace pd