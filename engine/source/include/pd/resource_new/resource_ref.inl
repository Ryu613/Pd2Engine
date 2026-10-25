#pragma once
#include "pd/resource_new/resource_manager.hpp"

namespace pd {
template <typename Tag>
ResourceRef<Tag>::ResourceRef(const ResourceRef& rhs)
    : mMgr(rhs.mMgr),
      mHandle(rhs.mHandle) {
  if (mMgr) {
    mMgr->retain(mHandle);
  }
}

template <typename Tag>
ResourceRef<Tag>::ResourceRef(ResourceRef&& rhs) noexcept
    : mMgr(std::exchange(rhs.mMgr, nullptr)),
      mHandle(std::exchange(rhs.mHandle, {})) {}

template <typename Tag>
ResourceRef<Tag>& ResourceRef<Tag>::operator=(const ResourceRef& rhs) {
  if (this != &rhs) {
    using std::swap;
    ResourceRef temp(rhs);
    swap(mMgr, temp.mMgr);
    swap(mHandle, temp.mHandle);
  }
  return *this;
}

// template <typename Tag>
// ResourceRef<Tag>& ResourceRef<Tag>::operator=(ResourceRef&& rhs) noexcept {
//   if (this != &rhs) {
//     reset();
//     mMgr = std::exchange(rhs.mMgr, nullptr);
//     mHandle = std::exchange(rhs.mHandle, {});
//   }
//   return *this;
// }

// template <typename Tag>
// void ResourceRef<Tag>::reset() noexcept {
//   if (!mMgr) {
//     return;
//   }

//   auto* manager = std::exchange(mMgr, nullptr);
//   auto handle = std::exchange(mHandle, {});

//   manager->release(handle);
// }

// template <typename Tag>
// TagInfo<Tag>::ResourceType* ResourceRef<Tag>::get() const noexcept {
//   return mMgr ? mMgr->get(mHandle) : nullptr;
// }
}  // namespace pd