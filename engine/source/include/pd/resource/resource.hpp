#pragma once

#include "pd/resource/resource_types.hpp"

namespace pd {
class Resource {
 public:
  virtual ~Resource();
  DELETE_COPY(Resource);
  DEFAULT_MOVABLE(Resource);

  Result<void> load() noexcept;
  Result<void> unload() noexcept;

  ResourceIdType id() const noexcept { return mId; }
  const std::string& name() const noexcept { return mName; }
  ResourceStatus status() const noexcept { return mStatus; }

 protected:
  explicit Resource(ResourceIdType id, const std::string& name);

  virtual Result<void> doLoad() noexcept = 0;
  virtual Result<void> doUnload() noexcept = 0;

 private:
  friend class ResourceManager;
  ResourceIdType mId = invalidResourceId;
  std::string mName;
  ResourceStatus mStatus = ResourceStatus::Unload;
};
}  // namespace pd