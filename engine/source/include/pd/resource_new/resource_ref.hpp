#pragma once

#include "pd/resource_new/resource_types.hpp"

namespace pd {
class ResourceManager;
template <typename Tag>
class ResourceRef {
 public:
  ResourceRef() = default;
  ~ResourceRef() { reset(); }
  ResourceRef(const ResourceRef& rhs);
  ResourceRef& operator=(const ResourceRef& rhs);
  ResourceRef(ResourceRef&& rhs) noexcept;
  ResourceRef& operator=(ResourceRef&& rhs) noexcept;

  void reset() noexcept;

  Result<void> load() noexcept;

  // 不打算给外部直接拿到本体
  //   TagInfo<Tag>::ResourceType* get() const noexcept;
  //   TagInfo<Tag>::ResourceType* operator->() const noexcept { return get(); }
  //   TagInfo<Tag>::ResourceType& operator*() const noexcept { return *get(); }

  ResourceHandle<Tag> handle() const noexcept { return mHandle; }

  explicit operator bool() const noexcept { return mMgr != nullptr && mHandle.isValid(); }

 private:
  friend class ResourceManager;
  ResourceManager* mMgr;

  ResourceHandle<Tag> mHandle;

  ResourceRef(ResourceManager& mgr)
      : mMgr(&mgr) {}
};
}  // namespace pd