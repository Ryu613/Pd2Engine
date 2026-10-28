#pragma once
#include "pd/resource/resource_manager.hpp"

namespace pd {
template <typename Tag>
ResourceRef<Tag>::ResourceRef(const ResourceRef& rhs)
    : mMgr(rhs.mMgr),
      mHandle(rhs.mHandle) {
  if (mMgr) {
    auto result = mMgr->retain(mHandle);
    PD_ASSERT(result);
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

template <typename Tag>
ResourceRef<Tag>& ResourceRef<Tag>::operator=(ResourceRef&& rhs) noexcept {
  if (this != &rhs) {
    reset();
    mMgr = std::exchange(rhs.mMgr, nullptr);
    mHandle = std::exchange(rhs.mHandle, {});
  }
  return *this;
}

template <typename Tag>
void ResourceRef<Tag>::reset() noexcept {
  if (!mMgr) {
    return;
  }

  auto* manager = std::exchange(mMgr, nullptr);
  auto handle = std::exchange(mHandle, {});

  auto result = manager->release(handle);
}

template <typename Tag>
Result<void> ResourceRef<Tag>::load() noexcept {
  if (!mMgr || !mHandle) {
    return {};
  }
  mMgr->load(mHandle);
}

// template <typename Tag>
// TagInfo<Tag>::ResourceType* ResourceRef<Tag>::get() const noexcept {
//   return mMgr ? mMgr->get(mHandle) : nullptr;
// }
}  // namespace pd