#pragma once

#include "pd/core/utils/mt/mt_queue.hpp"

#include <future>
#include <type_traits>

namespace pd::util {
class FuncWrapper {
 public:
  template <typename Func>
  FuncWrapper(Func&& f)
      : mImpl(new ImplType<Func>(std::move(f))) {}

  void operator()() { mImpl->call(); }
  FuncWrapper() = default;
  FuncWrapper(FuncWrapper&& rhs)
      : mImpl(std::move(rhs.mImpl)) {}

  FuncWrapper& operator=(FuncWrapper&& rhs) {
    mImpl = std::move(rhs.mImpl);
    return *this;
  }
  FuncWrapper(const FuncWrapper&) = delete;
  // 避免错误递归构造
  FuncWrapper(FuncWrapper&) = delete;
  FuncWrapper& operator=(const FuncWrapper&) = delete;

 private:
  struct ImplBase {
    virtual void call() = 0;
    virtual ~ImplBase() {}
  };
  std::unique_ptr<ImplBase> mImpl;

  template <typename Func>
  struct ImplType : ImplBase {
    Func func;
    ImplType(Func&& f)
        : func(std::move(f)) {}
    void call() override { func(); }
  };
};

class WorkStealingQueue {
  using DataType = FuncWrapper;

 public:
  WorkStealingQueue() {}
  WorkStealingQueue(const WorkStealingQueue& rhs) = delete;
  WorkStealingQueue& operator=(const WorkStealingQueue& rhs) = delete;

  void push(DataType data) {
    std::lock_guard<std::mutex> lock(mMut);
    mQueue.push_front(std::move(data));
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(mMut);
    return mQueue.empty();
  }

  bool tryPop(DataType& res) {
    std::lock_guard<std::mutex> lock(mMut);
    if (mQueue.empty()) {
      return false;
    }
    res = std::move(mQueue.front());
    mQueue.pop_front();
    return true;
  }

  bool trySteal(DataType& res) {
    std::lock_guard<std::mutex> lock(mMut);
    if (mQueue.empty()) {
      return false;
    }
    res = std::move(mQueue.back());
    mQueue.pop_back();
    return true;
  }

 private:
  std::deque<DataType> mQueue;
  mutable std::mutex mMut;
};

class JoinThreads {
 public:
  explicit JoinThreads(std::vector<std::thread>& threads)
      : mThreads(threads) {}
  ~JoinThreads() {
    for (auto& thread : mThreads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

 private:
  std::vector<std::thread>& mThreads;
};
class ThreadPool {
 public:
  ThreadPool()
      : mDone(false),
        mJoiner(mThreads) {
    const uint32_t threadCount = std::thread::hardware_concurrency();
    assert(threadCount != 0);
    try {
      for (uint32_t i = 0; i < threadCount; ++i) {
        mQueues.push_back(std::unique_ptr<WorkStealingQueue>(new WorkStealingQueue));
      }
      for (uint32_t i = 0; i < threadCount; ++i) {
        mThreads.push_back(std::thread(&ThreadPool::workerThread, this, i));
      }
    } catch (...) {
      mDone = true;
      throw;
    }
  }
  ~ThreadPool() { mDone = true; }
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  template <typename FuncType>
  std::future<std::invoke_result_t<FuncType()>> submit(FuncType func) {
    using ResultType = std::invoke_result_t<FuncType()>;

    std::packaged_task<ResultType()> task(std::move(func));
    std::future<ResultType> res(task.get_future());
    if (sLocalWorkQueue) {
      sLocalWorkQueue->push(std::move(task));
    } else {
      mWorkQueue.push(std::move(task));
    }
    return res;
  }

  void runPendingTask() {
    TaskType task;
    if (popTaskFromLocalQueue(task) || popTaskFromPoolQueue(task) || popTaskFromOtherThreadQueue(task)) {
      task();
    } else {
      std::this_thread::yield();
    }
  }

 private:
  using TaskType = FuncWrapper;

  std::atomic_bool mDone;
  mt::Queue<FuncWrapper> mWorkQueue;
  std::vector<std::unique_ptr<WorkStealingQueue>> mQueues;

  std::vector<std::thread> mThreads;
  JoinThreads mJoiner;

  using LocalQueueType = std::queue<FuncWrapper>;
  static thread_local WorkStealingQueue* sLocalWorkQueue;
  static thread_local uint32_t mIndex;

  void workerThread(uint32_t index) {
    mIndex = index;
    sLocalWorkQueue = mQueues[mIndex].get();
    while (!mDone) {
      runPendingTask();
    }
  };

  bool popTaskFromLocalQueue(TaskType& task) { return sLocalWorkQueue && sLocalWorkQueue->tryPop(task); }
  bool popTaskFromPoolQueue(TaskType& task) { return mWorkQueue.tryPop(task); }
  bool popTaskFromOtherThreadQueue(TaskType& task) {
    for (uint32_t i = 0; i < mQueues.size(); ++i) {
      const uint32_t index = (mIndex + i + 1) % mQueues.size();
      if (mQueues[index]->trySteal(task)) {
        return true;
      }
    }
    return false;
  }
};
}  // namespace pd::util