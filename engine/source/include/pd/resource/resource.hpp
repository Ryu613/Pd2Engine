#pragma once

#include <concepts>

namespace pd {
class Resource {
 public:
  using IdType = uint32_t;

  enum class Status : uint8_t {
    Unload = 1,
    Loading,
    Loaded,
  };
  Resource() noexcept = default;
  virtual ~Resource() = default;

  MOVABLE_ONLY(Resource);

  [[nodiscard]] const IdType& getId() const noexcept { return mId; }

  [[nodiscard]] bool isLoaded() const noexcept { return mStatus == Status::Loaded; }

  bool load() noexcept {
    log::debug("loading resource: {}", mId);
    mStatus = Status::Loading;
    mStatus = doLoad();
    return isLoaded();
  }

  void unload() noexcept {
    log::debug("unloading resource: {}", mId);
    mStatus = doUnload();
  }

 protected:
  virtual Status doLoad() noexcept = 0;
  virtual Status doUnload() noexcept = 0;

  explicit Resource(IdType id) noexcept
      : mId(id) {}

 private:
  friend class ResourceManager;

  IdType mId{};
  Status mStatus = Status::Unload;
};

template <typename T>
concept DerivedResource = std::derived_from<T, Resource>;
}  // namespace pd
