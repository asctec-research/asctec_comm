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

#include <asctec_comm/thread_safe_queue.h>
#include <asctec_comm/macros.h>

namespace asctec_comm
{

template<typename T>
ThreadSafeQueue<T>::ThreadSafeQueue(size_t maximumSize)
    : shutdownRequested_(false), maximumSize_(maximumSize)
{
}

template<typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue()
{
  shutdownRequested_ = true;
}

template<typename T>
bool ThreadSafeQueue<T>::empty() const
{
  UniqueLock lock(mutex_);
  return queue_.empty();
}

template<typename T>
size_t ThreadSafeQueue<T>::size() const
{
  UniqueLock lock(mutex_);
  return queue_.size();
}

template<typename T>
void ThreadSafeQueue<T>::push(T const& data)
{
  UniqueLock lock(mutex_);
  queue_.push_back(data);
  while(queue_.size() > maximumSize_)
  {
    queue_.pop_front();
    ASCTEC_WARN_STREAM("discarded element from ThreadSafeQueue");
  }
  lock.unlock();
  condition_.notify_one();
}

template<typename T>
template<class Iterator>
void ThreadSafeQueue<T>::push(const Iterator& first, const Iterator& last)
{
  UniqueLock lock(mutex_);
  queue_.insert(queue_.end(), first, last);

  // TODO: do this smarter
  while(queue_.size() > maximumSize_)
  {
    queue_.pop_front();
    ASCTEC_WARN_STREAM("discarded element from ThreadSafeQueue");
  }
  lock.unlock();
  condition_.notify_one();
}

template<typename T>
bool ThreadSafeQueue<T>::tryPop(T* item)
{
  UniqueLock lock(mutex_);
  if(queue_.empty())
  {
    return false;
  }

  if(item)
  {
    *item = queue_.front();
    queue_.pop_front();
  }
  return true;
}

template<typename T>
T ThreadSafeQueue<T>::pop()
{
  UniqueLock lock(mutex_);
  while(queue_.empty() && !shutdownRequested_)
  {
    condition_.wait(lock);
  }

  T item = queue_.front();
  queue_.pop_front();
  return item;
}

template<typename T>
template<class Rep, class Period>
bool ThreadSafeQueue<T>::popWithTimeout(const std::chrono::duration<Rep, Period>& timeout, T* item)
{
  std::cv_status status(std::cv_status::timeout);
  UniqueLock lock(mutex_);

  bool success = condition_.wait_for(lock, timeout, [this]
  { return !this->queue_.empty() && !this->shutdownRequested_;});

  if(!success)
  {
    return false;
  }

  if(item)
  {
    *item = queue_.front();
    queue_.pop_front();
  }
  return true;
}

template<typename T>
void ThreadSafeQueue<T>::clear()
{
  UniqueLock lock(mutex_);
  queue_.clear();
}

}
