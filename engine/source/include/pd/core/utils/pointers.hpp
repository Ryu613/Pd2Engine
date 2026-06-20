#pragma once

#include "pd/core/allocators.hpp"

namespace pd {
// TODO(author)
template <typename T, typename TArena = HeapAllocator>
class UPointer {
 public:
  UPointer() noexcept = default;
  ~UPointer() = default;
  MOVABLE_ONLY(UPointer);

  void deleter() noexcept {}

 private:
  TArena* mArena = nullptr;
  std::unique_ptr<T, deleter> mPtr;
};
}  // namespace pd