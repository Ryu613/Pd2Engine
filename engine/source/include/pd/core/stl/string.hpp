#pragma once

#include <string_view>
#include <cassert>

namespace pd {

namespace stl {
template <size_t Capacity, typename Arena>
class StringBuffer {
 public:
  StringBuffer(const StringBuffer&) = delete;
  StringBuffer& operator=(const StringBuffer&) = delete;
  StringBuffer(StringBuffer&& rhs) noexcept
      : mArena(std::exchange(rhs.mArena, nullptr)),
        mSize(std::exchange(rhs.mSize, 0)),
        mData(std::exchange(rhs.mData, nullptr)) {}

  StringBuffer& operator=(StringBuffer&& rhs) noexcept = delete;

  ~StringBuffer() noexcept = default;

  friend void swap(StringBuffer& lhs, StringBuffer& rhs) noexcept {
    using std::swap;
    swap(lhs.mArena, rhs.mArena);
    swap(lhs.mSize, rhs.mSize);
    swap(lhs.mData, rhs.mData);
  }

  [[nodiscard]] size_t size() const noexcept { return mSize; }
  [[nodiscard]] size_t capacity() const noexcept { return Capacity; }
  [[nodiscard]] bool empty() const noexcept { return mSize == 0; }
  [[nodiscard]] const char* data() const noexcept { return mData ? mData : &empty_str; }
  void clear() noexcept {
    mSize = 0;
    if (mData) {
      mData[0] = empty_str;
    }
  }

  explicit StringBuffer(Arena& arena, std::string_view str = {}) noexcept
      : mArena(&arena),
        mData(
            static_cast<char*>(mArena->alloc(sizeof(char) * (Capacity + 1), alignof(char)))) {
    assert(mData != nullptr);
    assert(str.size() <= Capacity);
    mData[0] = empty_str;
    if (str.empty()) {
      return;
    }
    mSize = str.size();
    memcpy(mData, str.data(), mSize);
    mData[mSize] = empty_str;
  }

  std::string_view append_n(std::string_view str) noexcept {
    assert(mData);
    assert(mSize + str.size() <= Capacity);
    if (str.empty()) {
      return view();
    }
    memcpy(mData + mSize, str.data(), str.size());
    mSize += str.size();
    mData[mSize] = empty_str;
    return {mData, mSize};
  }

  [[nodiscard]] std::string_view view() const noexcept {
    return mData ? std::string_view{mData, mSize} : std::string_view{};
  }

 private:
  static constexpr char empty_str = '\0';
  Arena* mArena = nullptr;
  size_t mSize = 0;
  char* mData = nullptr;
};
}  // namespace stl
}  // namespace pd