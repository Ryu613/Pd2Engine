#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include <numbers>

namespace pd {
namespace math {
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using quat = glm::quat;

inline constexpr f32 Pi = std::numbers::pi_v<f32>;
inline constexpr f32 PiOver2 = Pi / 2.0f;
inline constexpr f32 PiOver3 = Pi / 3.0f;
inline constexpr f32 PiOver4 = Pi / 4.0f;
inline constexpr f32 PiOver6 = Pi / 6.0f;

}  // namespace math
}  // namespace pd