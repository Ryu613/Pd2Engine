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

  BaseHandle() noexcept
      : mId(nullId),
        mGen(0) {}
  ~BaseHandle() = default;

  bool operator==(const BaseHandle& other) const {
    return mId == other.mId && mGen == other.mGen;
  }

  // 只判断有没有，不判断有效
  explicit operator bool() const noexcept { return mId != nullId; };
  // 判断是否有效
  [[nodiscard]] bool isValid() const noexcept { return mId != nullId && mGen != 0; };

  [[nodiscard]] HandleId id() const noexcept { return mId; }
  [[nodiscard]] uint32_t gen() const noexcept { return mGen; }

  // 拷贝和移动留给具体的句柄派生类实现，这里只有默认逻辑
 protected:
  BaseHandle(const BaseHandle&) noexcept = default;
  BaseHandle& operator=(const BaseHandle&) noexcept = default;
  BaseHandle(BaseHandle&& rhs) noexcept
      : mId(std::exchange(rhs.mId, nullId)),
        mGen(std::exchange(rhs.mGen, 0)) {}
  BaseHandle& operator=(BaseHandle&& rhs) noexcept {
    if (this != &rhs) {
      mId = rhs.mId;
      rhs.mId = nullId;
      mGen = rhs.mGen;
      rhs.mGen = 0;
    }
    return *this;
  }
  // 只能内部使用
  explicit BaseHandle(HandleId id, uint32_t gen) noexcept
      : mId(id),
        mGen(gen) {}

  void setId(HandleId id) noexcept { mId = id; }
  void setGen(uint32_t gen) noexcept { mGen = gen; }

 private:
  // 用于类型擦除并存储
  friend class ResourceManager;
  HandleId mId;
  uint32_t mGen;
};

template <typename T>
class TypedHandle : public BaseHandle {
 public:
  TypedHandle() noexcept = default;
  ~TypedHandle() = default;
  TypedHandle(const TypedHandle&) noexcept = default;
  TypedHandle(TypedHandle&& rhs) noexcept = default;
  TypedHandle& operator=(const TypedHandle& rhs) noexcept {
    BaseHandle::operator=(rhs);
    return *this;
  }
  TypedHandle& operator=(TypedHandle&& rhs) noexcept {
    BaseHandle::operator=(std::move(rhs));
    return *this;
  }

 protected:
  TypedHandle(HandleId id, uint32_t gen)
      : BaseHandle(id, gen) {}

 private:
  template <typename U, typename V>
  friend class Pool;
  friend class ResourceManager;
};

}  // namespace pd