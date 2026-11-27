#pragma once

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#include "volk.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vk_mem_alloc.h"

#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <cassert>
#include <memory>
#include <algorithm>
#include <span>

#ifdef ENABLE_LOG
#include <print>
#define LOG_TRACE(...) std::println("trace: " __VA_ARGS__)
#define LOG_DEBUG(...) std::println("debug: " __VA_ARGS__)
#define LOG_INFO(...) std::println("info: " __VA_ARGS__)
#define LOG_WARN(...) std::println("warn: " __VA_ARGS__)
#define LOG_ERROR(...) std::println("error: " __VA_ARGS__)
#endif

namespace vk1 {

namespace global {
inline constexpr uint32_t invalidIndex = UINT32_MAX;
inline constexpr size_t maxInflightFrames = 2;
inline constexpr uint32_t maxDescriptorPoolSize = 128;
inline constexpr uint32_t maxBindlessResources = 1024;
inline constexpr uint64_t defaultStagingBufferSize = 32 * 1024 * 1024;  // 32M
}  // namespace global

struct ImageState {
  VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  VkPipelineStageFlags2 pipelineStageMask = VK_PIPELINE_STAGE_2_NONE;
  VkAccessFlags2 accessMask = VK_ACCESS_2_NONE;
  VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  bool operator==(const ImageState&) const = default;
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