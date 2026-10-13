#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

namespace pd::util {

namespace mt {
/**
 * @brief 阻塞无界队列
 * @note 构造和析构需要使用者自行保证线程安全
 * @note T需要保证移动构造不抛异常
 * @warning 不要在T的构造函数和赋值操作符调用此容器的成员函数
 * @version 0.1
 */
template <typename T>
class Queue {
 public:
  Queue() = default;
  ~Queue() = default;
  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;
  Queue(Queue&&) = delete;
  Queue& operator=(Queue&&) = delete;

  void push(T newValue) {
    std::shared_ptr<T> data(std::make_shared<T>(std::move(newValue)));
    std::lock_guard<std::mutex> lk(mMut);
    mQueue.push(data);
    mDataCond.notify_one();
  }

  void waitAndPop(T& value) {
    std::unique_lock<std::mutex> lk(mMut);
    // wait for notify->wait for lock lk->check queue empty status
    // true: continue(with lk locked)
    // false: unlock lk and wait for notify again
    mDataCond.wait(lk, [this] { return !mQueue.empty(); });
    value = std::move(*mQueue.front());
    mQueue.pop();
  }

  std::shared_ptr<T> waitAndPop() {
    std::unique_lock<std::mutex> lk(mMut);
    mDataCond.wait(lk, [this] { return !mQueue.empty(); });
    std::shared_ptr<T> res = mQueue.front();
    mQueue.pop();
    return res;
  }

  bool tryPop(T& value) {
    std::lock_guard<std::mutex> lk(mMut);
    if (mQueue.empty()) {
      return false;
    }
    value = std::move(mQueue.front());
    mQueue.pop();
    return true;
  }

  std::shared_ptr<T> tryPop() {
    std::lock_guard<std::mutex> lk(mMut);
    if (mQueue.empty()) {
      return std::shared_ptr<T>{};
    }
    std::shared_ptr<T> res{std::make_shared<T>(std::move(mQueue.front()))};
    mQueue.pop();
    return res;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lk(mMut);
    return mQueue.empty();
  }

 private:
  mutable std::mutex mMut;
  std::queue<std::shared_ptr<T>> mQueue;
  std::condition_variable mDataCond;
};
}  // namespace mt
}  // namespace pd::util