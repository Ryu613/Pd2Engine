#pragma once

namespace pd {
class ILayer {
 public:
  enum class Error : uint8_t {
    InitializeFailed,
  };
  template <typename T>
  using LayerResult = Result<T, Error>;

  ILayer() noexcept = default;
  virtual ~ILayer() = default;
  MOVABLE_ONLY(ILayer);

 protected:
  virtual LayerResult<void> onAttached() noexcept { return {}; }
  virtual LayerResult<void> onDetached() noexcept { return {}; }
  virtual void onUpdate() noexcept {}

 private:
  friend class Engine;
};

template <typename T>
concept BaseOfLayer = std::derived_from<T, ILayer>;
}  // namespace pd