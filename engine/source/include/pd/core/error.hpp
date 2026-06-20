#pragma once

namespace pd {
enum class EngineError : uint8_t {
  InitializeFailed,
  RunFailed,
  LayerNotFound,
  LayerAttachFailed,
  LayerUpdateFailed,
  LayerDetachFailed,
  SceneLoadFailed,
  SceneUpdateFailed,
  SceneUnloadFailed,
};
}