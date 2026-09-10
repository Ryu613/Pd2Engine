#pragma once

#include "pd/resource/resource_types.hpp"

namespace pd {
class Backend;
class Resource {
 public:
  using IdType = u64;

  virtual ~Resource() = default;
  DELETE_COPY(Resource);
  DEFAULT_MOVABLE(Resource);

  Result<void> load(Backend& backend) noexcept;
  Result<void> unload(Backend& backend) noexcept;

 protected:
  virtual Result<void> doLoad(Backend& backend) noexcept = 0;
  virtual Result<void> doUnload(Backend& backend) noexcept = 0;

 private:
  friend class ResourceManager;
  IdType mId;
  std::string name;
  ResourceStatus mStatus;

  explicit Resource(IdType id);
};
}  // namespace pd