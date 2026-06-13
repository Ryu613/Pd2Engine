#pragma once

namespace pd {
class Layer {
 public:
  enum class Error : uint8_t {
    InitializeFailed,
  };
  template <typename T>
  using LayerResult = Result<T, Error>;

  Layer() noexcept = default;
  virtual ~Layer() = default;
  MOVABLE_ONLY(Layer);

 protected:
  virtual LayerResult<void> onAttached() noexcept { return {}; }
  virtual LayerResult<void> onDetached() noexcept { return {}; }
  virtual void onUpdate() noexcept {}

 private:
  friend class Engine;
};

template <typename T>
concept BaseOfLayer = std::derived_from<T, Layer>;
}  // namespace pd