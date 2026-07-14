#pragma once

namespace pd {
namespace stl {
// TODO(author)
template <typename T, uint32_t Capacity, typename Allocator>
class FixedVector {
 public:
 private:
  size_t mSize = 0;
  Allocator mAllocator;
  std::array<T, Capacity> mData;
};
}  // namespace stl
}  // namespace pd