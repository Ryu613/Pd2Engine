#pragma once

namespace pd {
class Handle {
 public:
  using HandleId = uint32_t;
  static constexpr HandleId nullId = HandleId{UINT32_MAX};

  Handle() noexcept
      : mId(nullId) {};

  [[nodiscard]] HandleId getId() const noexcept { return mId; }

  // 拷贝和移动留给具体的句柄派生类实现，这里只有默认逻辑
 protected:
  // 只能内部使用
  explicit Handle(HandleId id) noexcept
      : mId(id) {}
  Handle(const Handle&) = default;
  Handle& operator=(const Handle&) = default;
  Handle(Handle&& rhs) noexcept
      : mId(std::exchange(rhs.mId, nullId)) {}
  Handle& operator=(Handle&& rhs) noexcept { mId = std::exchange(rhs.mId, nullId); }
  ~Handle() noexcept = default;

  explicit operator bool() const noexcept { return mId != nullId; }
  bool operator==(const Handle& other) const { return mId == other.mId; }

 private:
  HandleId mId;
};

}  // namespace pd