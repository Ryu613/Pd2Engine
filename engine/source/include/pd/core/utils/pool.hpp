#pragma once

#include "pd/core/utils/handle.hpp"

namespace pd {
/**
 * @brief 存储同类资源, 管理生命周期, 用句柄作为资源凭据
 *
 * @tparam T
 * @tparam H
 */
template <typename T, typename THandleTag>
class Pool {
 public:
  template <typename TPool>
  using PoolType = std::vector<TPool>;

  explicit Pool(uint32_t capacity) noexcept
      : mCapacity(capacity) {
    mData.reserve(capacity);
    mGens.reserve(capacity);
    mFreeIndices.reserve(capacity);
  }
  Pool(const Pool&) = delete;
  Pool& operator=(const Pool&) = delete;
  Pool(Pool&& rhs) noexcept
      : mData(std::exchange(rhs.mData, {})),
        mGens(std::exchange(rhs.mGens, {})),
        mFreeIndices(std::exchange(rhs.mFreeIndices, {})),
        mCapacity(std::exchange(rhs.mCapacity, 0)) {}
  Pool& operator=(Pool&& rhs) noexcept {
    if (this != &rhs) {
      mData = std::exchange(rhs.mData, {});
      mGens = std::exchange(rhs.mGens, {});
      mFreeIndices = std::exchange(rhs.mFreeIndices, {});
      mCapacity = std::exchange(rhs.mCapacity, 0);
    }
    return *this;
  }
  ~Pool() = default;

  [[nodiscard]] uint32_t capacity() const noexcept { return mCapacity; };
  [[nodiscard]] uint32_t size() const noexcept {
    return mData.size() - mFreeIndices.size();
  };

  T* get(const TypedHandle<THandleTag>& handle) const noexcept {
    if (!isValidHandle(handle)) {
      return nullptr;
    }
    return const_cast<T*>(&mData[handle.id()]);
  }

  template <typename... Args>
  TypedHandle<THandleTag> emplace(Args&&... args) {
    if (size() >= mCapacity) {
      growCapacity();
    }
    TypedHandle<THandleTag> handle;
    auto handleId = handle.id();
    auto handleGen = handle.gen();
    // 有空位用空位，没有就加
    if (mFreeIndices.size() > 0) {
      handle.setId(mFreeIndices.back());
      mFreeIndices.pop_back();
      // 在清除时gen已经更新过了
      handle.setGen(mGens[handleId].gen);
      mGens[handleId].isAlive = true;
      mData[handleId] = T(std::forward<Args>(args)...);
    } else {
      mData.emplace_back(std::forward<Args>(args)...);
      mGens.emplace_back(GenerationEntry{1, true});
      handle.setId(mData.size() - 1);
      handle.setGen(1);
    }
    return handle;
  }

  TypedHandle<THandleTag> insert(const T& t) noexcept { return emplace(t); }

  /**
   * @brief 注：只在复用槽位时才会析构对象，此处只标记可复用
   *
   * @param handle
   */
  void remove(const TypedHandle<THandleTag>& handle) noexcept {
    if (!isValidHandle(handle)) {
      return;
    }
    auto handleId = handle.id();
    auto& entry = mGens[handleId];
    ++entry.gen;
    entry.isAlive = false;

    mFreeIndices.push_back(handleId);
  }

  /**
   * @brief 注：只标记可移除，不会直接析构，在复用时才会真正析构
   *
   */
  void clear() noexcept {
    for (size_t i = 0; i < mData.size(); ++i) {
      const auto handle = getNotAliveHandle(i);
      remove(handle);
    }
  }

  TypedHandle<THandleTag> getNotAliveHandle(uint32_t index) const noexcept {
    if (index >= mGens.size() || !mGens[index].isAlive) {
      return {};
    }
    return TypedHandle<THandleTag>{index, mGens[index].gen};
  }

 private:
  struct GenerationEntry {
    uint32_t gen = 0;
    bool isAlive = false;
  };
  PoolType<T> mData;
  PoolType<GenerationEntry> mGens;
  PoolType<uint32_t> mFreeIndices;
  uint32_t mCapacity = 0;

  bool isValidHandle(const TypedHandle<THandleTag>& handle) const noexcept {
    auto handleId = handle.id();
    auto handleGen = handle.gen();
    return handle.isValid() && handleId < mData.size() &&
           handleGen == mGens[handleId].gen && mGens[handleId].isAlive;
  }

  void growCapacity() noexcept;
};

template <typename T, typename H>
void Pool<T, H>::growCapacity() noexcept {
  mCapacity *= 2;
  PD_ASSERT(mCapacity < UINT32_MAX && mCapacity > 0);
  mData.reserve(mCapacity);
  mGens.reserve(mCapacity);
  mFreeIndices.reserve(mCapacity);
}
}  // namespace pd