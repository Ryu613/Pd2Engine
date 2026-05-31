#pragma once

namespace pd {
/**
 * @brief 代表渲染底层API的高层抽象，负责统一对不同API的调用差异，实现更方便的切换
 * 目前阶段RHI暂不实现，占位用
 *
 */
class Driver {
 public:
  Driver() noexcept = default;
  virtual ~Driver() = default;

  Driver(const Driver&) = delete;
  Driver& operator=(const Driver&) = delete;
  Driver(Driver&&) noexcept = delete;
  Driver& operator=(Driver&&) noexcept = delete;
};
}  // namespace pd