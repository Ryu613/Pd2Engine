#pragma once

#include "pd/core/utils/mt/mt_queue.hpp"

namespace pd::util {
class ThreadPool {
 public:
  ThreadPool()
      : mDone(false)
  // mJoiner
  {
    const uint32_t threadCount = std::thread::hardware_concurrency();
    assert(threadCount != 0);
    try {
      for (uint32_t i = 0; i < threadCount; ++i) {
        mThreads.push_back(std::thread(&ThreadPool::workerThread, this));
      }
    } catch (...) {
      mDone = true;
      throw;
    }
  }
  ~ThreadPool() { mDone = true; }

  template <typename FuncType>
  void submit(FuncType func) {
    mWorkQueue.push(std::function<void()>(func));
  }

 private:
  std::atomic_bool mDone;
  mt::Queue<std::function<void()>> mWorkQueue;
  std::vector<std::thread> mThreads;
  // join_threads mJoiner

  void workerThread() {
    while (!mDone) {
      std::function<void()> task;
      if (mWorkQueue.tryPop(task)) {
        task();
      } else {
        std::this_thread::yield();
      }
    }
  }
};
}  // namespace pd::util