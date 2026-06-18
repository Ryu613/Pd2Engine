#pragma once

namespace pd {
// TODO(author)
template <typename T, typename TArena>
class UPointer {
 public:
  UPointer() noexcept = default;
  ~UPointer() = default;
  MOVABLE_ONLY(UPointer);

 private:
  std::unique_ptr<T> mPtr;
};
}  // namespace pd