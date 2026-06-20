#pragma once

#include <concepts>

namespace pd {
class Resource {
 public:
  using IdType = std::string;

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

  void load() noexcept {
    log::debug("loading resource: {}", mId);
    mStatus = Status::Loading;
    doLoad();
    mStatus = Status::Loaded;
  }

  void unload() noexcept {
    log::debug("unloading resource: {}", mId);
    doUnload();
    mStatus = Status::Unload;
  }

  Status getStatus() const noexcept { return mStatus; }

 protected:
  virtual void doLoad() noexcept = 0;
  virtual void doUnload() noexcept = 0;

  explicit Resource(IdType id, Status status = Status::Unload) noexcept
      : mId(std::move(id)),
        mStatus(status) {}

 private:
  friend class ResourceManager;

  IdType mId;
  Status mStatus = Status::Unload;
};

template <typename T>
concept BaseOfResource = std::derived_from<T, Resource>;
}  // namespace pd
