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

  const IdType& getId() const noexcept { return mId; }

  bool isLoaded() const noexcept { return mStatus == Status::Loaded; }

  bool load() noexcept {
    log::debug("loading resource: {}", mName);
    mStatus = Status::Loading;
    mStatus = doLoad();
    return isLoaded();
  }

  void unload() noexcept {
    log::debug("unloading resource: {}", mName);
    mStatus = doUnload();
  }

 protected:
  virtual Status doLoad() noexcept = 0;
  virtual Status doUnload() noexcept = 0;

  explicit Resource(IdType id, std::string name, std::string path) noexcept
      : mId(std::move(id)),
        mName(std::move(name)),
        mPath(std::move(path)) {}

 private:
  IdType mId;
  std::string mName;
  std::string mPath;
  Status mStatus = Status::Unload;
};

template <typename T>
concept DerivedResource = std::derived_from<T, Resource>;
}  // namespace pd
