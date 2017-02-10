/*
 * Copyright (C) 2017 Intel Deutschland GmbH, Germany
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <deque>

namespace asctec_comm
{

template<typename T>
class ThreadSafeQueue
{
public:
  ThreadSafeQueue(size_t maximumSize);
  ~ThreadSafeQueue();
  bool empty() const;
  size_t size() const;

  void push(T const& data);

  template<class Iterator>
  void push(const Iterator& first, const Iterator& last);

  bool tryPop(T* item);
  T pop();

  template<class Rep, class Period>
  bool popWithTimeout(const std::chrono::duration<Rep, Period>& timeout, T* item);

  void clear();

private:
  typedef std::unique_lock<std::mutex> UniqueLock;
  std::deque<T> queue_;
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  size_t maximumSize_;
  bool shutdownRequested_;
};

}  // end namespace asctec_comm

#include <asctec_comm/implementation/thread_safe_queue_impl.h>
