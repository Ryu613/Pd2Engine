#pragma once

#include <concepts>

namespace pd {
class IBackend;
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

  void load(IBackend& backend) noexcept {
    log::debug("loading resource: {}", mId);
    if (mStatus != Status::Unload) {
      return;
    }
    mStatus = Status::Loading;
    doLoad(backend);
    mStatus = Status::Loaded;
  }

  void unload(IBackend& backend) noexcept {
    log::debug("unloading resource: {}", mId);
    if (mStatus != Status::Loaded) {
      return;
    }
    doUnload(backend);
    mStatus = Status::Unload;
  }

  [[nodiscard]] Status status() const noexcept { return mStatus; }

  [[nodiscard]] IdType id() const noexcept { return mId; }

 protected:
  virtual void doLoad(IBackend& backend) noexcept = 0;
  virtual void doUnload(IBackend& backend) noexcept = 0;

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
