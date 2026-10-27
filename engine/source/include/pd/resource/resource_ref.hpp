#pragma once

#include "pd/resource/resource_types.hpp"

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

  // 释放强引用，引用计数-1
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
  ResourceManager* mMgr = nullptr;

  ResourceHandle<Tag> mHandle{};

  ResourceRef(ResourceManager& mgr, ResourceHandle<Tag> handle)
      : mMgr(&mgr),
        mHandle(handle) {}
};
}  // namespace pd