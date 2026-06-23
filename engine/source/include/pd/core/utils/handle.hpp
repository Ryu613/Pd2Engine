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

  bool operator==(const BaseHandle& other) const {
    return mId == other.mId && mGen == other.mGen;
  }

  explicit operator bool() const noexcept { return mId != nullId; };
  [[nodiscard]] bool isValid() const noexcept { return mId != nullId && mGen != 0; };

  [[nodiscard]] HandleId id() const noexcept { return mId; }
  [[nodiscard]] uint32_t gen() const noexcept { return mGen; }

  // 拷贝和移动留给具体的句柄派生类实现，这里只有默认逻辑
 protected:
  // 只能内部使用
  explicit BaseHandle(HandleId id, uint32_t gen) noexcept
      : mId(id),
        mGen(gen) {}

  void setId(HandleId id) noexcept { mId = id; }
  void setGen(uint32_t gen) noexcept { mGen = gen; }

 private:
  // 用于类型擦除并存储
  friend class ResourceManager;
  HandleId mId = nullId;
  uint32_t mGen = 0;
};

template <typename T>
class TypedHandle : public BaseHandle {
 public:
  TypedHandle() noexcept = default;

 protected:
  TypedHandle(HandleId id, uint32_t gen)
      : BaseHandle(id, gen) {}

 private:
  template <typename U, typename V>
  friend class Pool;
  friend class ResourceManager;
};

}  // namespace pd