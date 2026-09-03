#pragma once

#include "pd/core/utils/memory.hpp"

namespace pd {
namespace util {
// template <typename T, size_t Capacity, typename Arena>
// class FixedCapacityVector {
//  public:
//   inline static size_t capacity = Capacity;
//   FixedCapacityVector() = default;
//   FixedCapacityVector(Arena& arena) noexcept
//       : mArena(&arena),
//         mData(mArena->alloc(capacity, alignof(T))) {}
//   ~FixedCapacityVector() noexcept {
//     if (mData) {
//       mArena->free(mData, capacity);
//     }
//   }
//   T& push_back(T t) noexcept {
//     if (mSize > capacity) {
//     }
//   }

//  private:
//   size_t mSize = 0;
//   T* mData = nullptr;
//   Arena* mArena = nullptr;
// };
}  // namespace util
}  // namespace pd