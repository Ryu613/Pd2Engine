#pragma once

#include <expected>

namespace pd {

enum class ErrorCode : u16 {
  SystemError = 1,
  EngineInitializeFailed = 100,
  EngineRunFailed,
  EngineResourceGCFailed,
  LayerNotFound = 200,
  LayerAttachFailed,
  LayerUpdateFailed,
  LayerDetachFailed,
  SceneLoadFailed = 300,
  SceneUpdateFailed,
  SceneUnloadFailed,
  RenderingSwapchainOutDate = 400,
  RenderingTimeout,
  RenderingNotImplemented,
  AssetFileNotFound = 500,
  AssetFileLoadError,
  AssetParseFailed,
  UnknownError = u16_max,
};

struct Error {
  ErrorCode code = ErrorCode::UnknownError;
  cstr msg = "unknown error";
};

template <typename Type>
using Result = std::expected<Type, Error>;

constexpr cstr default_msg(ErrorCode code) noexcept {
  switch (code) {
    using enum ErrorCode;
    case SystemError:
      return "System Error";
    case EngineInitializeFailed:
      return "Engine initialize failed";
    case EngineRunFailed:
      return "Engine run failed";
    case EngineResourceGCFailed:
      return "Engine GC failed";
    case LayerNotFound:
      return "Layer not found";
    case LayerAttachFailed:
      return "Layer attach failed";
    case LayerUpdateFailed:
      return "Layer update failed";
    case LayerDetachFailed:
      return "Layer detach failed";
    case SceneLoadFailed:
      return "Scene load failed";
    case SceneUpdateFailed:
      return "Scene update failed";
    case SceneUnloadFailed:
      return "Scene unload failed";
    case RenderingSwapchainOutDate:
      return "Swapchain out of date";
    case RenderingTimeout:
      return "Rendering timeout";
    case RenderingNotImplemented:
      return "Rendering not implemented";
    case AssetFileNotFound:
      return "Asset file not found";
    case AssetFileLoadError:
      return "Asset file load error";
    case AssetParseFailed:
      return "Asset parse failed";
    case UnknownError:
      return "Unknown error";
  }
  return "unhandled error!";
}

// note: prefer to use default msg, if not, do not use runtime string msg
template <typename T>
constexpr Result<T> make_error(ErrorCode code, cstr msg = nullptr) noexcept {
  cstr final_msg = msg ? msg : default_msg(code);
  return std::unexpected(Error{.code = code, .msg = final_msg});
}
}  // namespace pd