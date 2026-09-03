#pragma once

#include <expected>

namespace pd {

enum class ErrorCode : u16 {
  SystemError = 1,
  EngineError = 100,
  EngineInitializeFailed,
  EngineRunFailed,
  EngineResourceGCFailed,
  LayerError = 200,
  LayerNotFound,
  LayerAttachFailed,
  LayerUpdateFailed,
  LayerDetachFailed,
  SceneError = 300,
  SceneLoadFailed,
  SceneUpdateFailed,
  SceneUnloadFailed,
  RenderingError = 400,
  RenderingSwapchainOutDate,
  RenderingTimeout,
  RenderingNotImplemented,
  AssetError = 500,
  AssetFileNotFound,
  AssetFileLoadError,
  AssetParseFailed,
  AssetLoadFailed,
  ResourceError = 600,
  ResourceLoadFailed,
  WindowError = 700,
  WindowInitFailed,
  UnknownError = u16_max,
};

struct Error {
  ErrorCode code = ErrorCode::UnknownError;
  std::string_view msg = "unknown error";
};

template <typename Type>
using Result = std::expected<Type, Error>;

constexpr std::string_view default_msg(ErrorCode code) noexcept {
  switch (code) {
    using enum ErrorCode;
    case SystemError:
      return "System Error";
    case EngineError:
      return "engine error";
    case EngineInitializeFailed:
      return "Engine initialize failed";
    case EngineRunFailed:
      return "Engine run failed";
    case EngineResourceGCFailed:
      return "Engine GC failed";
    case LayerError:
      return "layer error";
    case LayerNotFound:
      return "Layer not found";
    case LayerAttachFailed:
      return "Layer attach failed";
    case LayerUpdateFailed:
      return "Layer update failed";
    case LayerDetachFailed:
      return "Layer detach failed";
    case SceneError:
      return "scene error";
    case SceneLoadFailed:
      return "Scene load failed";
    case SceneUpdateFailed:
      return "Scene update failed";
    case SceneUnloadFailed:
      return "Scene unload failed";
    case RenderingError:
      return "rendering error";
    case RenderingSwapchainOutDate:
      return "Swapchain out of date";
    case RenderingTimeout:
      return "Rendering timeout";
    case RenderingNotImplemented:
      return "Rendering not implemented";
    case AssetError:
      return "asset error";
    case AssetFileNotFound:
      return "Asset file not found";
    case AssetFileLoadError:
      return "Asset file load error";
    case AssetParseFailed:
      return "Asset parse failed";
    case AssetLoadFailed:
      return "Asset load failed";
    case ResourceError:
      return "resource error";
    case ResourceLoadFailed:
      return "resource load failed";
    case WindowError:
      return "window error";
    case WindowInitFailed:
      return "window init failed";
    case UnknownError:
      return "Unknown error";
  }
  return "unhandled error!";
}

// note: prefer to use default msg, if not, do not use runtime string msg
template <typename T>
constexpr Result<T> make_error(ErrorCode code, std::string_view msg = {}) noexcept {
  auto finalMsg = msg.empty() ? default_msg(code) : msg;
  return std::unexpected(Error{.code = code, .msg = finalMsg});
}
}  // namespace pd