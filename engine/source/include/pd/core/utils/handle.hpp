#pragma once

namespace pd {
/**
 * @brief 句柄基类
 *
 */
class BaseHandle {
 public:
  using HandleId = uint32_t;
  static constexpr HandleId nullId = HandleId{UINT32_MAX};

  BaseHandle() noexcept = default;

  explicit operator bool() const noexcept { return mId != nullId; };
  [[nodiscard]] bool isValid() const noexcept { return mId != nullId; };

  [[nodiscard]] HandleId id() const noexcept { return mId; }
  [[nodiscard]] uint32_t gen() const noexcept { return mGen; }

  // 拷贝和移动留给具体的句柄派生类实现，这里只有默认逻辑
 protected:
  // 只能内部使用
  explicit BaseHandle(HandleId id, uint32_t gen) noexcept
      : mId(id),
        mGen(gen) {}
  // 可拷可移
  BaseHandle(const BaseHandle&) = default;
  BaseHandle& operator=(const BaseHandle&) = default;
  BaseHandle(BaseHandle&& rhs) noexcept
      : mId(std::exchange(rhs.mId, nullId)),
        mGen(std::exchange(rhs.mGen, 0)) {}
  BaseHandle& operator=(BaseHandle&& rhs) noexcept {
    mId = std::exchange(rhs.mId, nullId);
    mGen = std::exchange(rhs.mGen, 0);
    return *this;
  }
  // 可非虚，不会通过此类delete对象
  ~BaseHandle() = default;
  bool operator==(const BaseHandle& other) const {
    return mId == other.mId && mGen == other.mGen;
  }

 private:
  HandleId mId = nullId;
  uint32_t mGen = 0;
};

template <typename T>
class TypedHandle : public BaseHandle {
 public:
  TypedHandle() noexcept = default;
  MOVABLE_ONLY(TypedHandle);
  ~TypedHandle() = default;

 protected:
  TypedHandle(HandleId id, uint32_t gen)
      : BaseHandle(id, gen) {}

 private:
  template <typename U, typename V>
  friend class Pool;
};

}  // namespace pd