#pragma once

namespace pd {
class Backend;
class ResourceManager {
 public:
  explicit ResourceManager(Backend* backend);
  ~ResourceManager();
  DELETE_COPY_MOVE(ResourceManager);

 private:
  Backend* mBackend = nullptr;
};
}  // namespace pd