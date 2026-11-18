#pragma once

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#include "volk.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vk_mem_alloc.h"

namespace vk1 {
namespace global {
inline constexpr uint32_t invalidIndex = UINT32_MAX;
inline constexpr size_t maxInflightFrames = 2;
inline constexpr uint32_t maxDescriptorPoolSize = 128;
inline constexpr uint32_t maxBindlessResources = 1024;
}  // namespace global

struct ImageState {
  VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  VkPipelineStageFlags2 pipelineStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 accessMask = VK_ACCESS_2_NONE;
  VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
};

enum RequestMode : uint8_t {
  Required,
  Optional,
};

enum ShaderLang : uint8_t {
  Slang,
};

struct Config {
  std::string appName = "vk1";
  std::unordered_map<std::string, RequestMode> instanceLayers;
  std::unordered_map<std::string, RequestMode> instanceExtensions;
  std::unordered_map<std::string, RequestMode> deviceExtensions;
  ShaderLang shaderLang = ShaderLang::Slang;
  bool enableSurface = false;
  bool enableDebug = false;
  bool enableHeadless = false;
  bool enableBindless = false;
};

}  // namespace vk1