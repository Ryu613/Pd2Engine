#pragma once

namespace pd {
using HandleIdType = u32;

/**
 * @brief 句柄
 *
 * 设计思路：纯数据，保持简单，内部使用，不过度封装访问性
 */
struct BaseHandle {
  using IdType = HandleIdType;
  inline static constexpr IdType invalidId = std::numeric_limits<IdType>::max();

  HandleIdType id = invalidId;
  u32 gen = 0;
};

template <typename T>
struct TypedHandle {
  BaseHandle data;

  friend bool operator==(TypedHandle lhs, TypedHandle rhs) noexcept {
    if (lhs.data.id == BaseHandle::invalidId && rhs.data.id == BaseHandle::invalidId) {
      return true;
    }
    return lhs.data.id == rhs.data.id && lhs.data.gen == rhs.data.gen;
  }

  friend bool operator!=(TypedHandle a, TypedHandle b) noexcept { return !(a == b); }

  // 只表示存在，不表示有效
  explicit operator bool() const noexcept { return data.id != BaseHandle::invalidId; }

  bool isValid() const noexcept { return data.id != BaseHandle::invalidId; }
};
}  // namespace pd