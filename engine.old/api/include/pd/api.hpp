#pragma once

namespace pd {
/**
 * @brief API接口基类，控制和规范Api的操作权限
 * 采用非虚下转，非pimpl
 * @todo 只展示思路，未完成
 */
class Api {
 public:
  Api(const Api&) = delete;
  Api& operator=(const Api&) = delete;
  Api(Api&&) noexcept = default;
  Api& operator=(Api&&) noexcept = default;

 protected:
  Api() noexcept = default;
  ~Api() = default;
};
}  // namespace pd