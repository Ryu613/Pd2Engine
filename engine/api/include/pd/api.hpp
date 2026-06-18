#pragma once

namespace pd {
/**
 * @brief API接口基类，控制和规范Api的操作权限
 * 采用非虚下转，非pimpl
 * @see filament的api设计
 * @todo 只展示思路，未完成
 */
class Api {
 public:
  Api() noexcept = default;
  ~Api() = default;
  MOVABLE_ONLY(Api);
};
}  // namespace pd